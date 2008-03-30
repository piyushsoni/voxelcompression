#include "Utilities.h"
#include "VirtualMachine.h"

namespace Pi
{
//Graphics3D: Created By Piyush Soni

//*******************************Class declarations*************************************************
class Vector3D;
class Point3D;



//**********************************common funtion declarations***********************
float distance(Point3D A, Point3D B);

float distance(float x1,float y1, float z1, float x2,float y2, float z2);

Point3D GetClosestPoint(Point3D P, Point3D PArray[], int Length);

Point3D GetClosestPoint(Point3D P, Point3D PArray[], int start, int end);

float VolumeOfTetrahedron(Point3D A, Point3D B, Point3D C, Point3D D);

Vector3D TriangleNormal(Point3D A, Point3D B, Point3D C);


//*******************************CLASS Point3D*************************************************
class Point3D
{
public:
  float x;
  float y;
  float z; 

  Point3D()
  {
    x=0;
    y=0;
	z=0;
  }

  Point3D(float x, float y, float z)
  {
    this->x=x;
    this->y=y;
	this->z=z;
  }

  Point3D TranslateBy(float dx,float dy, float dz)
  {
    x+=dx;
    y+=dy;
	z+=dz;
    return *this;
  }
  

  Point3D TranslateBy(Vector3D V);

  //translates only by a factor s of Vector3D V
  Point3D TranslateBy(Vector3D V, float s);


  Point3D TranslateTo(float xNew, float yNew, float zNew)
  {
    x=xNew;
    y=yNew;
	z=zNew;
    return *this;
  }
	
  Point3D Set(Point3D P)
  {
	  x=P.x;
	  y=P.y;
	  z=P.z;
	  return *this; 
  }

  void Print()
  {
	  std::cout<<"Point3D x: "<< x <<" y: "<< y<<" z: "<<z<<"\n";
  }


  void Print(char* Point3DName)
  {
    std::cout<<Point3DName<<".x= "<<x<< ", "<<Point3DName<<".y= "<<y<<", "<<Point3DName<<".z= "<<z<<"\n";
  }


  void Draw()
  {
    point(x,y,z);
  }
  
  void DrawSphere(float r)
  {
    sphere(x,y,z,r);
  }

  int IsTheSamePointAs(Point3D P)
  {
    return (AreSame(x,P.x) && AreSame(y,P.y) && AreSame(z,P.z));
  }

  int IsTheSamePointAs(Point3D P, float precision)
  {
    return (AreSame(x,P.x, precision) && AreSame(y,P.y, precision) && AreSame(z,P.z,precision));
  }
  


  Point3D ProjectionOnEdge(Point3D A, Point3D B);

  float DistanceToEdge(Point3D A, Point3D B);

  Vector3D VectorTo(Point3D B);

  float DistanceTo(Point3D P)
  {
    return (sqrt((x-P.x)*(x-P.x)+(y-P.y)*(y-P.y)+(z-P.z)*(z-P.z)));
  }

  Point3D MidPointWith(Point3D B)
  {
    return *(new Point3D((this->x+B.x)/2, (this->y+B.y)/2, (this->z+B.z)/2));
	
  }



  Point3D Clone()
  {
    return *(new Point3D(x,y,z));
  }
  
  void Vertex()
  {
    point(x,y,z); 
  }

};	
  Point3D Origin3D(0.0,0.0, 0.0);








//************************************************Vector3D CLASS***************************************************************
class Vector3D
{
public:
  float x, y,z; 

  Vector3D()
  {
	  x=0;
	  y=0;
	  z=0;
  }

  Vector3D(float x, float y, float z)
  {
    Set(x,y,z);
  }

  //a vector to points tail and head
  Vector3D(Vector3D Tail, Vector3D Head)
  {
    Set(Head.x-Tail.x, Head.y-Tail.y, Head.z-Tail.z);
  }

  Vector3D(Point3D Tail, Point3D Head)
  {
    Set(Head.x-Tail.x, Head.y-Tail.y, Head.z-Tail.z);
  }

  Vector3D Set(Vector3D Tail, Vector3D Head)
  {
    Set(Head.x-Tail.x, Head.y-Tail.y, Head.z-Tail.z);
    return *this;
  }

  Vector3D Set(Point3D Tail, Point3D Head)
  {
    Set(Head.x-Tail.x, Head.y-Tail.y, Head.z-Tail.z);
    return *this;
  }

  void Set(float x, float y, float z)
  {
    this->x=x;
    this->y=y;
	this->z=z;
  }

  Vector3D ScaleBy(float S)
  {
    x*=S;
    y*=S;
	z*=S;
    return *this;
  }

  /* will do it later.
  Vector3D RotateBy(float AngleRadian) 
  {
    //create new instances of the variables in memory
    float w=x; 
    float z=y; 
    x=w*cos(AngleRadian)-z*sin(AngleRadian); 
    y=w*sin(AngleRadian)+z*cos(AngleRadian); 
    return *this;
  }
*/

  Vector3D Normalize()
  {
    float value=this->Magnitude();
    x=x/value;
    y=y/value;
	z=z/value; 
    return *this;
  }


  Vector3D Invert()
  {
    
    if(y!=0) y*=-1; //There is a reason for doing that. otherwise it takes new value = -0.0, which gives weird results in angle calculations. 
    if(x!=0) x*=-1;
	if(z!=0) z*=-1; 
    return *this; 
  }
  
  Vector3D Add(Vector3D V)
  {
    x+=V.x;
    y+=V.y;
	z+=V.z; 
    return *this;
  }

  Vector3D AddScaled(Vector3D V, float s)
  {
    x+=s*V.x;
    y+=s*V.y;
	z+=s*V.z; 
    return *this;  
  }

  Vector3D Subtract(Vector3D V)
  {
    x-=V.x;
    y-=V.y;
	z-=V.z; 
    return *this;
  }


  float Magnitude()
  {
    return sqrt(x*x + y*y+ z*z);
  }


  Vector3D Clone()
  {
    return *(new  Vector3D(x,y,z));
  }


  float Dot(Vector3D V)
  {
    return (x*V.x + y*V.y + z*V.z);
  }

  Vector3D Cross(Vector3D V)
  {
    return *(new Vector3D(y*V.z-z*V.y, z*V.x-x*V.z, x*V.y-y*V.x));
  }

  Point3D HeadWhenTailAt(Point3D P)
  {
    return *(new Point3D(P.x+x,P.y+y, P.z+z));
  }
  
  Point3D TailWhenHeadAt(Point3D P)
  {
    return *(new  Point3D(P.x-x,P.y-y, P.z-z));
  }

  void DrawTailAt(Point3D P, int DrawArrow)
  {
    line(P.x, P.y, P.z, P.x+this->x, P.y+this->y, P.z+this->z);
	//this will have to sit for a while .. has to be a cone here. 
	/*
    if(DrawArrow)
    {
      Point3D A;
		Point3D PHead(P.x+this->x, P.y+this->y);
		Line3D Arrow(P, PHead);
      float ArrowLength=(sqrt(this->x*this->x+this->y*this->y))/10;
      
	  if(this->x==0) A.TranslateTo(P.x,PHead.y - (y/abs(y))*ArrowLength);
      else
      A.Set(Arrow.Point3DAtDistanceFrom(PHead, -x/abs(x)*ArrowLength));
      
      Point3D P1= Arrow.PerpendicularThrough(A).Point3DAtDistanceFrom(A, ArrowLength/3);
      Point3D P2= Arrow.PerpendicularThrough(A).Point3DAtDistanceFrom(A, -ArrowLength/3);
      line(P1.x,P1.y, P2.x,P2.y);
      line(P1.x,P1.y, PHead.x,PHead.y);
      line(P2.x,P2.y, PHead.x,PHead.y);
    }
	*/
  }

  void DrawHeadAt(Point3D P, int DrawArrow)
  {
    line(P.x-this->x, P.y-this->y, P.z-this->z, P.x, P.y, P.z);
	/*
    if(DrawArrow)
    {
      Point3D A;
	Line3D Arrow(P, *(new Point3D(P.x-this->x, P.y-this->y)));
      float ArrowLength=(sqrt(this->x*this->x+this->y*this->y))/10;
      
	  if(this->x==0) A.TranslateTo(P.x,P.y - (y/abs(y))*ArrowLength);
      else
      A.Set(Arrow.Point3DAtDistanceFrom(P, -x/abs(x)*ArrowLength));
      Point3D P1= Arrow.PerpendicularThrough(A).Point3DAtDistanceFrom(A, ArrowLength/3);
      Point3D P2= Arrow.PerpendicularThrough(A).Point3DAtDistanceFrom(A, -ArrowLength/3);
      line(P1.x,P1.y, P2.x,P2.y);
      line(P1.x,P1.y, P.x,P.y);
      line(P2.x,P2.y, P.x,P.y);    
    }
	*/
  }

  void Print()
  {
	  std::cout<<"Vector3D x: "<<x<<" y: "<<y<<" z: "<<z<<"\n";
  }




};




//************************************************Member Function Definitions***************************************************************
Point3D Point3D::TranslateBy(Vector3D V)
  {
    x+=V.x;
    y+=V.y;
	z+=V.z; 
    return *this;
  }

  //translates only by a factor s of Vector3D V
Point3D Point3D::TranslateBy(Vector3D V, float s)
  {
    x+=s*V.x;
    y+=s*V.y;
	z+=s*V.z; 
    return *this;
  }
  
  Vector3D Point3D::VectorTo(Point3D B)
  {
    return *(new  Vector3D(*this, B));
  }


//************************************************COMMON FUNCTIONS***************************************************************





float distance(Point3D A, Point3D B)
{
  return sqrt(sq(A.x-B.x)+sq(A.y-B.y)+sq(A.z-B.z));

}

float distance(float x1,float y1, float z1,float x2,float y2, float z2)
{
  return sqrt(sq(x1-x2)+sq(y1-y2)+sq(z1-z2));
}



//Only the Length number of items are checked
Point3D GetClosestPoint(Point3D P, Point3D PArray[], int Length)
{
  return GetClosestPoint(P, PArray, 0,Length-1);
}


Point3D GetClosestPoint(Point3D P, Point3D PArray[], int start, int end)
{
  Point3D res=PArray[start];
  float d=distance(P,PArray[start]);
  float temp;
  for(int i=start+1;i<=end;i++)
  {
    temp=distance(P,PArray[i]);
    if(temp<d) 
    {
      d=temp;
      res=PArray[i];

    }
  }
  return res;
}


float VolumeOfTetrahedron(Point3D A, Point3D B, Point3D C, Point3D D)
{
  Vector3D V1(A,B);
  Vector3D V2(A,C);
  Vector3D V3(A,D);
  return V1.Cross(V2).Dot(V3)/6;
}

Vector3D TriangleNormal(Point3D A, Point3D B, Point3D C)
{
	Vector3D P(A,B);
	Vector3D Q(A,C);
	return P.Cross(Q);
	
}



}