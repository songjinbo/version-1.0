// GetVoxelThread.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include <queue>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <octomap/octomap.h>
#include <octomap/octomap_timing.h>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include "Dialog.h"
#include "DialogDlg.h"
#include "GetVoxelThread.h"
#include "GetImageThread.h"

IMPLEMENT_DYNCREATE(GetVoxelThread, CWinThread)

GetVoxelThread::GetVoxelThread()
{
}

GetVoxelThread::~GetVoxelThread()
{
}

BOOL GetVoxelThread::InitInstance()
{
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int GetVoxelThread::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(GetVoxelThread, CWinThread)
	ON_THREAD_MESSAGE(WM_GETVOXEL_BEGIN, GetVoxel)
END_MESSAGE_MAP()

using namespace cv;
using namespace std;
using namespace octomap;

string itos(double);//�ⲿ����

extern CCriticalSection critical_rawdata; //���߳�1��get_image�����Ľӿ�
extern vector<Mat> vec_depth;
extern vector<Mat> vec_left;
extern vector<Position> vec_position;
extern volatile get_image_ret_code get_image_status;

extern CCriticalSection critical_single_rawdata;//�����߳�display_image�����Ľӿ�
extern cv::Mat depth_image;
extern cv::Mat left_image;
extern Position position;
extern volatile ProgressStatus progress_status;

extern vector<double> voxel_x;//���߳�PathPlan����ά��ʾ�Ľӿ�
extern vector<double> voxel_y;
extern vector<double> voxel_z;

extern int count_voxel_file;

volatile get_voxel_ret_code get_voxel_status = get_voxel_is_running;//���ⲿ�Ľӿڣ���־��GetVoxel��״̬


const double u0 = 116.502;
const double v0 = 156.469;
const double fx = 239.439;
const double fy = 239.439;
char save_file[MAX_PATH] = { "./data/depth.log" }; //��Ҫ�������logFilename����һ��

//·��
string logFilename = "./data/depth.log";
string graphFilename = "./data/depth.graph";
string treeFilename = "./data/depth.bt";
string vrmlFilename;
string txtFilename;

double res = 0.4; //resolution  
double maxrange = 10;  //default:-1
int max_scan_no = -1;  //default:-1
bool detailedLog = false; //enable a detailed log file with statistics  default:false
bool simpleUpdate = false; //simple scan insertion ray by ray instead of optimized  default:false
bool discretize = false; //approximate raycasting on discretized coordinates, speeds up insertion  default:false
bool dontTransformNodes = false; //nodes are already in global coordinates and no transformation is required  default:false
unsigned char compression = 1; //enable maximum-likelihood compression (lossy) after every scan (=2)  default:1

// get default sensor model values:(it can be changed)
OcTree emptyTree(0.1);
double clampingMin = emptyTree.getClampingThresMin(); //override default sensor model clamping probabilities between 0..1
double clampingMax = emptyTree.getClampingThresMax();
double probMiss = emptyTree.getProbMiss(); //override default sensor model hit and miss probabilities between 0..1
double probHit = emptyTree.getProbHit();

void calcThresholdedNodes(const OcTree* tree,
	unsigned int& num_thresholded,
	unsigned int& num_other)
{
	num_thresholded = 0;
	num_other = 0;

	for (OcTree::tree_iterator it = tree->begin_tree(), end = tree->end_tree(); it != end; ++it){
		if (tree->isNodeAtThreshold(*it))
			num_thresholded++;
		else
			num_other++;
	}
}

void outputStatistics(const OcTree* tree){   //show size of tree
	unsigned int numThresholded, numOther;
	calcThresholdedNodes(tree, numThresholded, numOther);
	size_t memUsage = tree->memoryUsage();
	unsigned long long memFullGrid = tree->memoryFullGrid();
	size_t numLeafNodes = tree->getNumLeafNodes();

	double x, y, z;
	tree->getMetricSize(x, y, z);
}

void GetVoxelThread::GetVoxel(UINT wParam, LONG lParam)
{
	get_voxel_status = get_voxel_is_running; //���ñ�־λ
	
	//��queue��ȡ��һ֡����

	if (!vec_depth.empty() && !vec_left.empty() && !vec_position.empty())
	{
		critical_rawdata.Lock();
		critical_single_rawdata.Lock();

		std::ofstream outfile(save_file);

		while (!vec_depth.empty() && !vec_left.empty() && !vec_position.empty())
		{
			left_image = vec_left.front();
			vec_left.erase(vec_left.begin());
			position = vec_position.front();
			vec_position.erase(vec_position.begin());
			depth_image = vec_depth.front();
			vec_depth.erase(vec_depth.begin());

			critical_rawdata.Unlock();

			outfile << "NODE " << position.z << " " << position.x << " " << -position.y << " " << position.yaw << " " << position.roll << " " << -position.pitch << "\n";
			for (int u = 0; u < depth_image.rows; u += 2){
				for (int v = 0; v < depth_image.cols; v += 2){

					//������ӵĴ���
					if (progress_status == is_stopped)
					{
						critical_single_rawdata.Unlock();
						get_voxel_status = get_voxel_is_stopped;
						::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, get_voxel_is_stopped, NULL);
						return;
					}

					double Xw = 0, Yw = 0, Zw = 0;
					Zw = depth_image.at<short>(u, v) / 128.0;

					Xw = (u - u0) * Zw / fx;
					Yw = -(v - v0) * Zw / fy;

					if (Zw < 1) continue;

					outfile << Xw << " " << Yw << " " << Zw << "\n";
				}
			}
		}
		outfile.close();
		critical_single_rawdata.Unlock();
	}

	else
	{
		get_voxel_status = no_data_in_queue;
		::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, get_voxel_status, NULL);
		return;
	}
	//�������е����ݵ�����������
	
	//Reading Log file
	ScanGraph* graph = new ScanGraph();
	graph->readPlainASCII(logFilename);
	size_t num_points_in_graph = 0;
	if (max_scan_no > 0) {
		num_points_in_graph = graph->getNumPoints(max_scan_no - 1);
	}
	else {
		num_points_in_graph = graph->getNumPoints();
	}

	// transform pointclouds first, so we can directly operate on them later
	if (!dontTransformNodes) {
		for (ScanGraph::iterator scan_it = graph->begin(); scan_it != graph->end(); scan_it++) {

			//������ӵĴ���
			if (progress_status == is_stopped)
			{
				get_voxel_status = get_voxel_is_stopped;
				::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, get_voxel_status, NULL);
				return;
			}

			pose6d frame_origin = (*scan_it)->pose;
			point3d sensor_origin = frame_origin.inv().transform((*scan_it)->pose.trans());

			(*scan_it)->scan->transform(frame_origin);
			point3d transformed_sensor_origin = frame_origin.transform(sensor_origin);
			(*scan_it)->pose = pose6d(transformed_sensor_origin, octomath::Quaternion());

		}
	}

	std::ofstream logfile;
	if (detailedLog){
		logfile.open((treeFilename + ".log").c_str());
		logfile << "# Memory of processing " << graphFilename << " over time\n";
		logfile << "# Resolution: " << res << "; compression: " << int(compression) << "; scan endpoints: " << num_points_in_graph << std::endl;
		logfile << "# [scan number] [bytes octree] [bytes full 3D grid]\n";
	}

	//Creating tree
	OcTree* tree = new OcTree(res);

	// tree set
	tree->setClampingThresMin(clampingMin);
	tree->setClampingThresMax(clampingMax);
	tree->setProbHit(probHit);
	tree->setProbMiss(probMiss);

	timeval start;
	timeval stop;
	gettimeofday(&start, NULL);  // start timer
	size_t numScans = graph->size();
	size_t currentScan = 1;

	// insertPointCloud
	for (ScanGraph::iterator scan_it = graph->begin(); scan_it != graph->end(); scan_it++) {

		//������ӵĴ���
		if (progress_status == is_stopped)
		{
			get_voxel_status = get_voxel_is_stopped;
			::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, get_voxel_status, NULL);
			return;
		}

		if (simpleUpdate)
			tree->insertPointCloudRays((*scan_it)->scan, (*scan_it)->pose.trans(), maxrange);
		else
		{
			tree->insertPointCloud((*scan_it)->scan, (*scan_it)->pose.trans(), maxrange, false, discretize);
		}

		if (compression == 2){
			tree->toMaxLikelihood();
			tree->prune();
		}

		if (detailedLog)
			logfile << currentScan << " " << tree->memoryUsage() << " " << tree->memoryFullGrid() << "\n";

		if ((max_scan_no > 0) && (currentScan == (unsigned int)max_scan_no))
			break;

		currentScan++;
	}

	gettimeofday(&stop, NULL);  // stop timer
	double time_to_insert = (stop.tv_sec - start.tv_sec) + 1.0e-6 *(stop.tv_usec - start.tv_usec);

	// get rid of graph in mem before doing anything fancy with tree (=> memory)
	delete graph;
	if (logfile.is_open())
		logfile.close();

	tree->toMaxLikelihood();
	tree->prune();
	outputStatistics(tree);

	//Writing tree files
	tree->writeBinary(treeFilename);

	//write wrl
	vrmlFilename = "./data/depth.bt.wrl";
	std::ofstream outfile(vrmlFilename.c_str());
	outfile << "#VRML V2.0 utf8\n#\n";
	outfile << "# created from OctoMap file " << treeFilename << " with bt2vrml\n";

	size_t count(0);
	for (OcTree::leaf_iterator it = tree->begin(), end = tree->end(); it != end; ++it) {
		if (tree->isNodeOccupied(*it)){
			count++;
			double size = it.getSize();
			double tmp1 = it.getX(), tmp2 = it.getY(), tmp3 = it.getZ();
			outfile << "Transform { translation "    //vrml
				<< it.getX() << " " << it.getY() << " " << it.getZ()
				<< " \n  children ["
				<< " Shape { geometry Box { size "
				<< size << " " << size << " " << size << "} } ]\n"
				<< "}\n";
		}
	}
	outfile.close();

	//write txt
	//txtFilename = treeFilename + itos(count_voxel_file) + ".txt";
	txtFilename = treeFilename + ".txt";

	count_voxel_file++; //�����ݲű�������ļ�

	std::ofstream outfilet(txtFilename.c_str());
	for (OcTree::leaf_iterator it = tree->begin(), end = tree->end(); it != end; ++it) {

		//������ӵĴ���
		if (progress_status == is_stopped)
		{
			get_voxel_status = get_voxel_is_stopped;
			::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, get_voxel_status, NULL);
			return;
		}

		if (tree->isNodeOccupied(*it)){
			double size = it.getSize();
			if (size == res)
			{
				//outfilet << it.getX() << " " << it.getY() << " " << it.getZ() << " " << size << " " << size << " " << size << "\n";
				outfilet << it.getX() << " " << it.getY() << " " << it.getZ() << "\n";
				voxel_x.push_back(int(it.getX() * 10) / 10.0); //it.getX()���浽�ļ��ٶ�������к��ֵ������it.getX(),���ǵ���int(it.getX()*10)/10.0
				voxel_y.push_back(int(it.getY() * 10) / 10.0);
				voxel_z.push_back(int(it.getZ() * 10) / 10.0);
			}
			else
			{
				if (size == 2 * res)
				{
					int i, j, k;
					double xyz[] = { it.getX(), it.getY(), it.getZ() };
					for (i = -1; i <= 1; i = i + 2)
						for (j = -1; j <= 1; j = j + 2)
							for (k = -1; k <= 1; k = k + 2)
							{
								//outfilet << it.getX() + i*res*0.5 << " " << it.getY() + j*res*0.5 << " " << it.getZ() + k*res*0.5 << " " << res << " " << res << " " << res << "\n";
								outfilet << it.getX() + i*res*0.5 << " " << it.getY() + j*res*0.5 << " " << it.getZ() + k*res*0.5 << "\n";
								voxel_x.push_back(int((it.getX() + i*res*0.5) * 10) / 10.0);
								voxel_y.push_back(int((it.getY() + j*res*0.5) * 10) / 10.0);
								voxel_z.push_back(int((it.getZ() + k*res*0.5) * 10) / 10.0);
							}
				}
				if (size == 4 * res)
				{
					int i, j, k, l, m, n;
					double xyz[] = { it.getX(), it.getY(), it.getZ() };
					for (i = -1; i <= 1; i = i + 2)
						for (j = -1; j <= 1; j = j + 2)
							for (k = -1; k <= 1; k = k + 2)
							{
								double xl, yl, zl;
								xl = it.getX() + i*res;
								yl = it.getY() + j*res;
								zl = it.getZ() + k*res;
								for (l = -1; l <= 1; l = l + 2)
									for (m = -1; m <= 1; m = m + 2)
										for (n = -1; n <= 1; n = n + 2)
										{
											//outfilet << xl + l*res*0.5 << " " << yl + m*res*0.5 << " " << zl + n*res*0.5 << " " << res << " " << res << " " << res << "\n";
											outfilet << xl + l*res*0.5 << " " << yl + m*res*0.5 << " " << zl + n*res*0.5 << "\n";
											voxel_x.push_back(int((xl + l*res*0.5) * 10) / 10.0);
											voxel_z.push_back(int((zl + n*res*0.5) * 10) / 10.0);
											voxel_y.push_back(int((yl + m*res*0.5) * 10) / 10.0);
										}
							}
				}

			}
		}
	}
	delete tree;
	outfilet.close();

	get_voxel_status = get_all_voxel_complete;
	::PostMessage((HWND)(GetMainWnd()->GetSafeHwnd()), WM_DISPLAY_IMAGE, get_voxel_status, NULL);
	return;
}