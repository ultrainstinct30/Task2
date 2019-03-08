#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <bits/stdc++.h>

#define rows img.rows
#define cols img.cols
#define ll long long int

using namespace cv;
using namespace std;

struct point{
	int x;                   //x represents the xth row
	int y;                   //y represents the yth column
};
/*struct Line{
	float a;
	float b;
	float c;
	//ax+by+c=0
};*/
struct Edge{
	point p1; //this is the parent node.
	point p2; //this is the child node.
};

Mat img = imread("Test1.png",1);
Mat vis(rows,cols,CV_8UC1, Scalar(0));
Mat cpy = img.clone();
Mat path;

int length = 0;

vector<point> nodes;
vector<Edge> edges;
vector<point> qu;
vector<point> nearVec;
vector<float> costVec(4000);
vector<point> pathPoint;

int findedge(point p){
	for(int i=0;i<edges.size(); i++){
		if(edges[i].p2.x == p.x && edges[i].p2.y == p.y) return i;
	}
}
bool isvalid(point p){
	if((p.x<0 || p.x>=rows) || (p.y<0 || p.y>cols))
		return 0;
	else if(img.at<Vec3b>(p.x,p.y)[0]>230 && img.at<Vec3b>(p.x,p.y)[1]>230 && img.at<Vec3b>(p.x,p.y)[2]>230){
		return 0;
	}
	else{
		return 1;
	}
}
/*bool lineSatisfy(float theta, pointp3){
	
		return 1;
	return 0;
}
Line eqnOfLine(point p1, point p2){
	if(p1.y - p2.y == 0)
		return {0, 1, (float)(-p1.y)};
	return {1, (float)((p1.x-p2.x)/(p2.y-p1.y)), (float)((p1.x*p2.y-p2.x-p1.y)/(p1.y-p2.y))};
}*/
float dist(point A, point B){
	int dx = A.x-B.x;
	int dy = A.y - B.y;
	return sqrt(dx*dx + dy*dy);
}
bool isValid(Edge v){
	float magn = dist(v.p1, v.p2), x, y;
	x = (v.p2.x-v.p1.x)/magn;
	y = (v.p2.y-v.p1.y)/magn;
	for(int i=1; i<=10;i++){
		point tmp = {v.p1.x + i*x, v.p1.y + i*y};
		if(!isvalid(tmp)) return 0;
	}
	return 1;
}
point randompoint(){
	point random;
	random.x = rand()%rows;
	random.y = rand()%cols;
	return random;
}
point nearestpoint(point rand){
	float min = 100000000000;
	point near;
	for(auto u: nodes){
		if(dist(u, rand) < min){
			near = u;
			min = dist(u, rand);
		}
	}
	return near;
}
point steer(point np, point rp){
	float minDist = dist(np, rp);
	point newpoint;
	float magn = dist(np, rp), x, y;
	x = (float)(rp.x-np.x)/magn;
	y = (float)(rp.y-np.y)/magn;
	newpoint.x = np.x + x*10;
	newpoint.y = np.y + y*10;
	return newpoint;
}
/*void binary(){
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if(img.at<Vec3b>(i,j)[0]>100 && img.at<Vec3b>(i,j)[1]>100 && img.at<Vec3b>(i,j)[2]>100)
				img.at<Vec3b>(i,j)={255,255,255};
			if(img.at<Vec3b>(i,j)[0]<100 && img.at<Vec3b>(i,j)[1]<100 && img.at<Vec3b>(i,j)[2]<100)
				img.at<Vec3b>(i,j)={0,0,0};
			if(img.at<Vec3b>(i,j)[0]<100 && img.at<Vec3b>(i,j)[1]<100 && img.at<Vec3b>(i,j)[2]>200)
				img.at<Vec3b>(i,j)={0,0,255};
			if(img.at<Vec3b>(i,j)[0]<100 && img.at<Vec3b>(i,j)[1]>200 && img.at<Vec3b>(i,j)[2]<100)
				img.at<Vec3b>(i,j)={0,255,0};
		}
	}
}*/
point centre(int chnl){
	Mat img2=img.clone();
	int sumx=0, sumy=0, ctr=0;
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			if(img.at<Vec3b>(i,j)[0]>=220 && img.at<Vec3b>(i,j)[1]>=220 && img.at<Vec3b>(i,j)[2]>=220){
				img2.at<Vec3b>(i,j)[0]=0;
				img2.at<Vec3b>(i,j)[1]=0;
				img2.at<Vec3b>(i,j)[2]=0;
			}
		}
	}
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			if(img2.at<Vec3b>(i,j)[chnl] >= 230){
				sumx += i;
				sumy += j;
				ctr++;
			}
		}
	}	
	point centre = {sumx/ctr, sumy/ctr};
	return centre;
}
void coloredge(Edge v){
	line(cpy, Point(v.p1.y, v.p1.x), Point(v.p2.y, v.p2.x), Scalar(255,0,255), 1, 8);	
}
void eraseedge(Edge v){
	line(cpy, Point(v.p1.y, v.p1.x), Point(v.p2.y, v.p2.x), Scalar(0,0,0), 1, 8);	
}
void setOfNearPoints(point newNode, float radius){
	nearVec.clear();
	for(int i=-(int)radius; i<(int)radius+1; i++){
		for(int j=-(int)radius; j<(int)radius+1; j++){
			point p = {newNode.x+i, newNode.y+j};
			for(int i=0; i<nodes.size(); i++){
				if(p.x == nodes[i].x && p.y == nodes[i].y && dist(p, newNode)<=radius) nearVec.push_back(p);
			}
		}
	}
}
point parent(point p){
	for(int i=edges.size()-1; i>=0; i++){
		if(edges[i].p2.x == p.x && edges[i].p2.y == p.y) return edges[i].p1;
	}
}
float costOfNode(point p){
	for(int i=0; i<nodes.size(); i++){
		if(nodes[i].x == p.x && nodes[i].y == p.y) return costVec[i];
	}
}
void costAssign(point p, int i){
	costVec[i] = costOfNode((parent(p))) + dist((parent(p)), p);
}
void costChange(point p,int i, float d){
	costVec[i] = costVec[i] + d;
}
void minCostPath(point newNode, point nearest){
	float minCost = costOfNode(nearest) + dist(newNode, nearest);
	point near = nearest;
	for(int i=0; i<nearVec.size(); i++){
		if(isValid({nearVec[i], newNode})){
			if(costOfNode(nearVec[i]) + dist(nearVec[i], newNode) <  minCost){
				near = nearVec[i];
				minCost = costOfNode(nearVec[i]) + dist(nearVec[i], newNode);
			}
		}
	}
	coloredge({near, newNode});
	edges.push_back({near, newNode});
}
int findIndex(point p){
	for(int i=0; i<nodes.size(); i++){
		if(nodes[i].x==p.x && nodes[i].y==p.y) return i;
	}
}
void costChangeChildren(point p,int j, float diff){
	for(int i=0; i<edges.size(); i++){
		if(edges[i].p1.x == p.x && edges[i].p1.y == p.y){
			int k = findIndex(edges[i].p2);
			costChange(p,j, diff);	
			costChangeChildren(edges[i].p2,k, diff);	
			return;
		}
	}
	costChange(p,j, diff);
	return;
}
void rewire(point newNode){
	for(int i=0; i<nearVec.size(); i++){
		if(isValid({newNode, nearVec[i]})){
			if(costOfNode(newNode) + dist(newNode, nearVec[i]) < costOfNode(nearVec[i])){
				int j = findIndex(nearVec[i]);
				eraseedge(edges[findedge(nearVec[i])]);
				edges.erase(edges.begin()+findedge(nearVec[i]));
				edges.push_back({newNode, nearVec[i]});
				//cout<<edges[edges.size()-1].p1.x<<" "<<edges[edges.size()-1].p1.y<<" "<<edges[edges.size()-1].p2.x<<" "<<edges[edges.size()-1].p2.y<<"\n";
				coloredge({newNode, nearVec[i]});
				float diff = costOfNode(newNode) + dist(newNode, nearVec[i]) - costOfNode(nearVec[i]);
				costChange(nearVec[i],j,diff);
				//costChangeChildren(nearVec[i],j, diff);
				}
		}
	}
}
void findPath(Edge v){
	if(v.p1.x == nodes[0].x && v.p1.y == nodes[0].y){
		line(cpy, Point(v.p1.y, v.p1.x), Point(v.p2.y, v.p2.x), Scalar(0,255,0), 1, 8);
		length += dist(v.p1, v.p2);
		pathPoint.push_back(v.p1);
		imshow("RRTStar", cpy);
		// cout<<"Reached starting point\n";pathPoint.push_back(tmp.p2);
		return;
	}
	else{
		for(int i=edges.size()-1; i>=0; i--){
			if(v.p1.x == edges[i].p2.x && v.p1.y == edges[i].p2.y){
				line(cpy, Point(v.p1.y, v.p1.x), Point(v.p2.y, v.p2.x), Scalar(0,255,0), 1, 8);
				length += dist(v.p1, v.p2);
				imshow("RRTStar", cpy);
				pathPoint.push_back(v.p1);
				findPath(edges[i]);
			}
		}
	}
}
int main(){
	namedWindow("RRTStar", WINDOW_NORMAL);
	srand(time(0));
	point init = centre(1);
	nodes.push_back(init);
	costVec[0] = 0;
	edges.push_back({init, init});
	for(int i=1; ; i++){
		cout<<"<<<<    "<<i<<"    >>>>>\n";
		point rand = randompoint();
		point nearestNode = nearestpoint(rand);
		point newNode = steer(nearestNode, rand);
		if(!isvalid(newNode) || vis.at<uchar>(newNode.x, newNode.y) == 255){
			i--;
			continue;
		}
		if(!isValid({nearestNode, newNode})){
			i--;
			continue;
		}
		setOfNearPoints(newNode, 20);
		//for(auto u: nearVec) cout<<u.x<<" "<<u.y<<endl;
		nodes.push_back(newNode);
		vis.at<uchar>(newNode.x, newNode.y) = 255;
		cpy.at<Vec3b>(newNode.x, newNode.y) = {0,255,255};
		//printf("<<<<<<Iteration-%d>>>>>>>\nRandom Point generated : (%d, %d)\nnearestNode is (%d, %d)\nnewNode is (%d, %d)\n\n\n", i, rand.x, rand.y, nearestNode.x, nearestNode.y, newNode.x, newNode.y);
		minCostPath(newNode, nearestNode);
		//cout<<edges[edges.size()-1].p1.x<<" "<<edges[edges.size()-1].p1.y<<" "<<edges[edges.size()-1].p2.x<<" "<<edges[edges.size()-1].p2.y<<"\n";
		costAssign(newNode, i);
		rewire(newNode);
		imshow("RRTStar", cpy);
		waitKey(1);
		if(img.at<Vec3b>(newNode.x, newNode.y)[2]>230)	break;
		//if(i==3000) return 0;
	}
	Edge tmp = *(edges.end()-1);
	//cout<<tmp.p2.x<<" "<<tmp.p2.y<<"\n";
	pathPoint.push_back(tmp.p2);
	//pathPoint.push_back(tmp);
	// int j=0;
	// int length = 0;
	cout<<"\n\n\n<<<<<<<<<Geneated Path>>>>>>>>>>\n\n\n";
	/*cout<<edges.size()<<"\n";
	waitKey(0);*/
	/*for(int i = edges.size()-1; i>=0; i--){
		if(edges[i].p2.x == tmp.x && edges[i].p2.y == tmp.y){
			tmp = edges[i].p1;
			//cout<<"iteration no"<<++j;
			pathPoint.push_back(tmp);
			cpy.at<Vec3b>(edges[i].p2.x, edges[i].p2.y) = {0,255,0};
			length += dist(edges[i].p1, edges[i].p2);
			line(cpy, Point {edges[i].p1.y, edges[i].p1.x}, Point {edges[i].p2.y, edges[i].p2.x}, Scalar(0,255,0), 1, 8);
			imshow("RRTStar", cpy);
			waitKey(1);
		}
	}*/
	findPath(tmp);
	ofstream myfile;
	int j=0;
	myfile.open("points.txt");
	for(int i=pathPoint.size()-1; i>=0; i--){ 
		cout<<++j<<". ("<<pathPoint[i].y<<","<<pathPoint[i].x<<")\n";
		if(i>0){
			myfile<<pathPoint[i].y<<"\n"<<rows-pathPoint[i].x<<"\n";
		}
	}
	myfile.close();
	cout<<"\n\n\nPath Length = "<<length<<"\n";
	cout<<rows<<" "<<cols"\n";
	waitKey(0);
	return 0;
}
