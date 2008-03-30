#include "Utilities.h"
#include "VirtualMachine.h"

namespace Pi
{
//Graphics2D: Created By Piyush Soni

//*******************************Class declarations*************************************************
class Vector;
class Line; 
class Intersection; 
class Point;



//**********************************common funtion declarations***********************
float distance(Point A, Point B);

float distance(float x1,float y1,float x2,float y2);

float distance(Line L);

int IsPointInTriangle(float x,float y,float Tx1,float Ty1,float Tx2,float Ty2,float Tx3,float Ty3);

int IsPointInTriangle(Point P,Point T1,Point T2,Point T3);

Point GetClosestPoint(Point P, Point PArray[], int Length);

Point GetClosestPoint(Point P, Point PArray[], int start, int end);

float AngleToDraw(Point P);

float AngleToDraw(Vector V);

float AngleToDraw(float y,float x);

float CalculateSignedArea(Point P[], int n);



//*******************************CLASS POINT*************************************************
class Point
{
public:
  float x;
  float y;

  Point()
  {
    x=0;
    y=0;
  }

  Point(float x, float y)
  {
    this->x=x;
    this->y=y;
  }

  Point TranslateBy(float dx,float dy)
  {
    x+=dx;
    y+=dy;
    return *this;
  }
  

  Point TranslateBy(Vector V);

  //translates only by a factor s of vector V
  Point TranslateBy(Vector V, float s);


  Point TranslateTo(float xNew, float yNew)
  {
    x=xNew;
    y=yNew;
    return *this;
  }
	
  Point Set(Point P)
  {
	  x=P.x;
	  y=P.y;
	  return *this; 
  }

  void Print()
  {
	  std::cout<<"Point x: "<< x <<" y: "<< y<<"\n";
  }


  void Print(char* PointName)
  {
	  std::cout<<PointName<<".x= "<<x<< ", "<<PointName<<".y= "<<y<<"\n";
  }


  void Draw()
  {
    point(x,y);
  }
  
  void DrawCircle(float r)
  {
    ellipse(x,y,2*r,2*r);
  }

  int IsTheSamePointAs(Point P)
  {
    return (AreSame(x,P.x) && AreSame(y,P.y));
  }

  int IsTheSamePointAs(Point P, float precision)
  {
    return (AreSame(x,P.x, precision) && AreSame(y,P.y, precision));
  }
  
  int IsInCircumcircle(Point A, Point B, Point C);


  int IsFacingEdge(Point A, Point B);

  Point ProjectionOnEdge(Point A, Point B);
  
  Point ProjectionOnEdge(Line L);

  float DistanceToEdge(Point A, Point B);

  float DistanceToEdge(Line L);

  Vector VectorTo(Point B);

  float DistanceTo(Point P)
  {
    return (sqrt((x-P.x)*(x-P.x)+(y-P.y)*(y-P.y)));
  }

  Point MidPointWith(Point B)
  {
    return *(new Point((this->x+B.x)/2, (this->y+B.y)/2));
  }



  Point Clone()
  {
    return *(new Point(x,y));
  }
  
  void Vertex()
  {
    point(x,y); 
  }

};	
  Point Origin(0.0,0.0);





//*****************CLASS LINE****************************************************
class Line
{
public: 
  Point a;
  Point b;
  int IsLineVertical;

  Line()
  {
    IsLineVertical=false;
  }
  Line(Point P1, Point P2)
  {
    a=P1;
    b=P2;
    if(P1.x==P2.x) IsLineVertical=(1);
    else IsLineVertical=false;
  }
  
  Line(float x1, float y1, float x2, float y2)
  {
Point     a(x1, y1);
Point     b(x2, y2);
    IsLineVertical=false;
  }

  void TranslateBy(float dx, float dy)
  {
    a.TranslateBy(dx,dy);
    b.TranslateBy(dx,dy);
  }

  Point MidPoint()
  {
    Point res; 
    res.x=(a.x+b.x)/2;
    res.y=(a.y+b.y)/2;
    return res;
  }

  void TranslateMidPointTo(Point P)
  {
    Point M;
	M=this->MidPoint();
    this->TranslateBy(P.x-M.x,P.y-M.y);
  }

  float Length()
  {
    return sqrt(sq(b.y-a.y)+sq(b.x-a.x));
  }

  float Slope()
  {
    if(AreSame(b.x,a.x))
    {
      IsLineVertical=(1);
      return infinity;
    }
    IsLineVertical=false;
    return (b.y-a.y)/(b.x-a.x);

  }

  Point PointAtDistanceFrom(Point P, float r)
  {
    float m=this->Slope();//just to make sure the variable IsLineVertical has some value
    if(IsLineVertical)
      return *new Point(P.x,P.y+r);
    else
    {
      return *new Point(P.x+r*cos(atan(this->Slope())), P.y+r*sin(atan(this->Slope())));
    }

  }

  Line PerpendicularThrough(Point P)
  {
    Line res;//just to make sure the variable IsLineVertical has some value
    res.a=P;
    float m=this->Slope();
    res.b.x=P.x+50;    
    if(IsLineVertical)
    {
      //The new line will be horizontal
      res.b.y=P.y;
      return res;
    }
    //Equation of new line will be x+my=c, where c=P.y+ P.x/m, as it passes through P
    res.b.y=P.y-50/m;
    return res;

  }

  Line ParallelThrough(Point P)
  {
    return this->PerpendicularThrough(P).PerpendicularThrough(P); // inefficient method ... have to correct it :)
  }

  Line Clone()
  {
    return *(new  Line(a.Clone(), b.Clone()));
  }

  int IsParallelTo(Line L)
  {
    
    if(AreSame(this->Slope(),L.Slope())) return (1); 
    else return false;
  }
  
  int IsPerpendicularTo(Line L)
  {
    if(AreSame(-1/this->Slope(), L.Slope())) return (1); else return false;
  }

  void Draw()
  {
    line(a.x, a.y, b.x,b.y);
  }

  void Print()
  {
	  std::cout<<"The two Line is made by the points: \n";
    std::cout<<"*****************\n";
    this->a.Print();
    this->b.Print();
	std::cout<<"*****************\n";
    
  }

};



//****************************CLASS TRIANGLE************************************************
class Triangle
{
public: 
  Line L1,L2,L3;
  Point T1,T2,T3;

  Triangle()
  {
Line     L1;
Line     L2;
Line     L3;
Point     T1;
Point     T2;
Point     T3;
    L1.a=T1;
    L1.b=T2;
    L2.a=T2;
    L2.b=T3;
    L3.a=T3;
    L3.b=T1;
  }

  Triangle(Point P1, Point P2, Point P3)
  {
Line     L1;
Line     L2;
Line     L3;
    T1=P1;
    T2=P2;
    T3=P3;
    L1.a=T1;
    L1.b=T2;
    L2.a=T2;
    L2.b=T3;
    L3.a=T3;
    L3.b=T1;
  }

  Triangle(Line ln1, Line ln2, Line ln3);


  void SetVertices(Point P1, Point P2, Point P3)
  {
    T1=P1;
    T2=P2;
    T3=P3;
    L1.a=T1;
    L1.b=T2;
    L2.a=T2;
    L2.b=T3;
    L3.a=T3;
    L3.b=T1;
  }

  void TranslateBy(float dx, float dy)
  {
    /*L1.TranslateBy(dx,dy);
     L2.TranslateBy(dx,dy);
     L3.TranslateBy(dx,dy);*/
    T1.TranslateBy(dx,dy);
    T2.TranslateBy(dx,dy);
    T3.TranslateBy(dx,dy);

  }

  Point MidPoint()
  {
	Point res;
    res.x=(T1.x+T2.x+T3.x)/3;
    res.y=(T1.y+T2.y+T3.y)/3;
    return res;
  }

  void TranslateMidPointTo(Point P)
  {
    Point M=this->MidPoint();
    this->TranslateBy(P.x-M.x,P.y-M.y);
  }

  void Draw()
  {
    triangle(T1.x,T1.y,T2.x,T2.y,T3.x,T3.y);
  }

  Triangle Clone()
  {
    return *(new  Triangle(T1.Clone(),T2.Clone(),T3.Clone()));
  }
  
  float SignedArea()
  {
    return 0.5*(T1.x*T2.y-T1.y*T2.x+ T3.x*T1.y-T3.y*T1.x+T2.x*T3.y-T3.x*T2.y); // -ve included in the result as what in real world is positive, becomes negative area here. y inverted
  }
};


class Intersection
{
public: 
  Point POI;
  int IsIntersectionOnlyOnExtrapolation;
  int AreLinesParallel;
  int AreLinesIdentical;
public:
  Intersection()
  {
Point     POI;
    POI.x=-999;
    POI.y=-999;
    AreLinesParallel=false;
    AreLinesIdentical=false;
    IsIntersectionOnlyOnExtrapolation=false;
  }

  Point returnIntersection(Line L1, Line L2)
  {
    POI=returnIntersection(L1.a,L1.b,L2.a,L2.b);
    return POI;
  }

  Point returnIntersection(Point L1a,Point L1b,Point L2a,Point L2b)
  {
    POI=returnIntersection(L1a.x,L1a.y,L1b.x,L1b.y,L2a.x,L2a.y,L2b.x,L2b.y);
    return POI;
  }


  Point returnIntersection(float L1x1,float L1y1,float L1x2,float L1y2,float L2x1,float L2y1,float L2x2,float L2y2)
  {
    float m1,m2,c1,c2;//slopes and constants of line L1 and L2 to express them in the form y=mx+c
    int ToReturn=false;
	Line L1;
	Line L2;
    L1.a.x=L1x1;
    L1.a.y=L1y1;
    L1.b.x=L1x2;
    L1.b.y=L1y2;
    L2.a.x=L2x1;
    L2.a.y=L2y1;
    L2.b.x=L2x2;
    L2.b.y=L2y2;


    //Test Boundary cases/ Exceptional cases
    //1). If One or both of the Lines are vertical  
    if(AreSame(L1x1,L1x2)) //Is line 1 vertical
    {
      if(AreSame(L2x1,L2x2)) // Is line 2 vertical
      {
        AreLinesParallel=(1);
        if(AreSame(L2x1,L1x1))
        {
          AreLinesIdentical=(1);
        }
      }
      else //Line 1 is vertical but Line 2 is not vertical
      {
        m2=(L2y2-L2y1)/(L2x2-L2x1);
        c2=(L2x2*L2y1-L2y2*L2x1)/(L2x2-L2x1);
        POI.x=L1x1;
        POI.y=m2*L1x1+c2;
        ToReturn=(1);
      }

    }
    else if(AreSame(L2x1,L2x2))
    {
      //We've already tested, both lines are not vertical together, that means Line1 is not vertical
      m1=(L1y2-L1y1)/(L1x2-L1x1);
      c1=(L1x2*L1y1-L1y2*L1x1)/(L1x2-L1x1);
      POI.x=L2x1;
      POI.y=m1*L2x1+c1;
      ToReturn=(1);
    }




    //If ToReturn is false, none of the lines is vertical.
    // That means m and c can be finitely calculated. 
    m1=(L1y2-L1y1)/(L1x2-L1x1);
    m2=(L2y2-L2y1)/(L2x2-L2x1);
    c1=(L1x2*L1y1-L1y2*L1x1)/(L1x2-L1x1);
    c2=(L2x2*L2y1-L2y2*L2x1)/(L2x2-L2x1);


    //2). Testing if the lines are parallel
    if(!ToReturn)
    {

      if(AreSame(m1,m2))
      {
        //Means lines are parallel
        AreLinesParallel=(1);
        if(AreSame(c1,c2)) AreLinesIdentical=(1); //since in the form y=mx+c, both lines have same slope and constant
        ToReturn=(1);
      }
    }


    //If ToReturn is still false, that means the lines are neither parallel nor Identical
    //3). That means it will have finite Point of Intersection
    if(!ToReturn)
    {
      POI.x=(c2-c1)/(m1-m2);
      POI.y=(m1*c2-m2*c1)/(m1-m2);
      ToReturn=(1);
    }
    //Now, the only remaining thing to be found out is - whether the LINE SEGMENTS actually intersect - The point of intersection is not on Extrapolation - 

      if(!AreLinesParallel)
    {
      if(distance(L1.a,POI)>distance(L1) || distance(L1.b,POI)>distance(L1) || distance(L2.b,POI)>distance(L2) || distance(L2.a,POI)>distance(L2))
        IsIntersectionOnlyOnExtrapolation=(1);     
    }
    return POI;
  }


};


//**********************************************CLASS ARC********************************
class Arc
{
private :
  Point Center;
  float radiusX,radiusY;
  float Angle[100]; // for future when an array of points can be inserted. 
public:
	int CantBeFormed;
  Point Start,End;//Limits of the arc - through which it passes
  Arc()
  {
    
Point     Start;
Point     End;
    CantBeFormed=(1);//Since the values are not initialized right now
  }

  //Meant to draw arc from Start to End in the Anticlockwise direction in real world, but since in computer screen y is down, it's clockwise

  Arc(Point C, Point Start, Point End)
  {
    Set(C,Start,End);

  }

public:
  void SetPoints(Point Center, Point Start, Point End)
  {
    Set(Center,Start,End);
  }

private :
  void Set(Point C, Point Start, Point End)
  {

    this->Start=Start;
    this->End=End;
    this->Center=C;
    radiusX=Center.DistanceTo(Start);
    if(!AreSame(radiusX,Center.DistanceTo(End)))
      CantBeFormed=(1);
    else

    {
      CantBeFormed=false;
      radiusY=radiusX;
      Angle[0]=AngleToDraw((Start.y-Center.y),(Start.x-Center.x));// A function which gives angle from 0 to 2*PI
      Angle[1] = AngleToDraw((End.y-Center.y),(End.x-Center.x));
      

      //The following lines are not needed now. 
      /*
    if(Angle[0]>Angle[1]) 
       {
       // InterChange the Angles - To ensure that Angle[0]<Angle[1]
       float Temp=0;
       Temp=Angle[0];
       Angle[0]=Angle[1];
       Angle[1]=Temp;
       }
       */

    }
  }




  void Draw()
  {

    if(!CantBeFormed)
    {
      
      arc(Center.x,Center.y,2*radiusX,2*radiusY,Angle[0],Angle[1]);

    }

  }



};



//************************************************VECTOR CLASS***************************************************************
class Vector
{
public:
  float x, y;

  Vector()
  {
  }

  Vector(float x, float y)
  {
    Set(x,y);
  }

  Vector(Vector Tail, Vector Head)
  {
    Set(Head.x-Tail.x, Head.y-Tail.y);
  }

  Vector(Point Tail, Point Head)
  {
    Set(Head.x-Tail.x, Head.y-Tail.y);
  }

  Vector Set(Vector Tail, Vector Head)
  {
    Set(Head.x-Tail.x, Head.y-Tail.y);
    return *this;
  }

  Vector Set(Point Tail, Point Head)
  {
    Set(Head.x-Tail.x, Head.y-Tail.y);
    return *this;
  }

  void Set(float x, float y)
  {
    this->x=x;
    this->y=y;
  }

  Vector ScaleBy(float S)
  {
    x*=S;
    y*=S;
    return *this;
  }

  //Rotates a vector IN THE ANTICLOCKWISE DIRECTION in paper, but in computer screen as y axis is down, rotates CLOCKWISE. 
  Vector RotateBy(float AngleRadian) 
  {
    //create new instances of the variables in memory
    float w=x; 
    float z=y; 
    x=w*cos(AngleRadian)-z*sin(AngleRadian); 
    y=w*sin(AngleRadian)+z*cos(AngleRadian); 
    return *this;
  }

  Vector Normalize()
  {
    float value=this->Magnitude();
    x=x/value;
    y=y/value;
    return *this;
  }

//*****************************NOTE::::::    in real world, the right and left functions would be different. But in screen, the y axis is inverted, so this change
  Vector Right()
  {
    //create new float variable
    float z;
    z=-y;
    y=x;
    x=z;  
    return *this;
  }

  Vector Left()
  {
    //create new float variable
    float z;
    z=y;
    y=-x;
    x=z;
    return *this;
  }

  Vector Invert()
  {
    
    if(y!=0) y*=-1; //There is a reason for doing that. otherwise it takes new value = -0.0, which gives weird results in angle calculations. 
    if(x!=0) x*=-1;
    return *this; 
  }
  
  Vector Add(Vector V)
  {
    x+=V.x;
    y+=V.y;
    return *this;
  }

  Vector AddScaled(Vector V, float s)
  {
    x+=s*V.x;
    y+=s*V.y;
    return *this;  
  }

  Vector Subtract(Vector V)
  {
    x-=V.x;
    y-=V.y;
    return *this;
  }


  float Magnitude()
  {
    return sqrt(x*x + y*y);
  }


  Vector Clone()
  {
    return *(new  Vector(x,y));
  }

  //Essentially returns the Anticlockwise Angle from 0 to 2PI in paper, but in computer screen as y axis is down, returns clockwise. 
  float AngleToTurnFor(Vector B)
  {
    if(AngleToDraw(B)>AngleToDraw(*this))
      return AngleToDraw(B)-AngleToDraw(*this);
    else
      return 2*PI-(AngleToDraw(*this)-AngleToDraw(B));

  }

  //Returns Angle To Turn for B, from minus PI to Plus PI.
  float AngleToTurnForPlusMinusPI(Vector B)
  {
    float res=AngleToTurnFor(B);
    if(res>PI) return -(2*PI-res);
    else return res;
    
  }

  float Dot(Vector V)
  {
    return (x*V.x+y*V.y);
  }

  int IsPointOnLeft(Point StartingPointOfVector, Point P)
  {
	  return this->Clone().Left().Dot(*(new Vector(StartingPointOfVector, P)))>0;

  }

  Point HeadWhenTailAt(Point P)
  {
    return *(new  Point(P.x+x,P.y+y));
  }
  
  Point TailWhenHeadAt(Point P)
  {
    return *(new  Point(P.x-x,P.y-y));
  }

  void DrawTailAt(Point P, int DrawArrow)
  {
    line(P.x, P.y, P.x+this->x, P.y+this->y);
    if(DrawArrow)
    {
      Point A;
		Point PHead(P.x+this->x, P.y+this->y);
		Line Arrow(P, PHead);
      float ArrowLength=(sqrt(this->x*this->x+this->y*this->y))/10;
      
	  if(this->x==0) A.TranslateTo(P.x,PHead.y - (y/abs(y))*ArrowLength);
      else
      A.Set(Arrow.PointAtDistanceFrom(PHead, -x/abs(x)*ArrowLength));
      
      Point P1= Arrow.PerpendicularThrough(A).PointAtDistanceFrom(A, ArrowLength/3);
      Point P2= Arrow.PerpendicularThrough(A).PointAtDistanceFrom(A, -ArrowLength/3);
      line(P1.x,P1.y, P2.x,P2.y);
      line(P1.x,P1.y, PHead.x,PHead.y);
      line(P2.x,P2.y, PHead.x,PHead.y);
    }
  }

  void DrawHeadAt(Point P, int DrawArrow)
  {
    line(P.x-this->x, P.y-this->y, P.x, P.y);
    if(DrawArrow)
    {
      Point A;
	Line Arrow(P, *(new Point(P.x-this->x, P.y-this->y)));
      float ArrowLength=(sqrt(this->x*this->x+this->y*this->y))/10;
      
	  if(this->x==0) A.TranslateTo(P.x,P.y - (y/abs(y))*ArrowLength);
      else
      A.Set(Arrow.PointAtDistanceFrom(P, -x/abs(x)*ArrowLength));
      Point P1= Arrow.PerpendicularThrough(A).PointAtDistanceFrom(A, ArrowLength/3);
      Point P2= Arrow.PerpendicularThrough(A).PointAtDistanceFrom(A, -ArrowLength/3);
      line(P1.x,P1.y, P2.x,P2.y);
      line(P1.x,P1.y, P.x,P.y);
      line(P2.x,P2.y, P.x,P.y);    
    }
  }

  void Print()
  {
    std::cout<<"Vector x: "<<x<<" y: "<<y<<"\n";
  }




};



//************************************************POLYLOOP CLASS***************************************************************
class Polyloop
{
  static const int MaxSupported=5000; //Max number of points
  int n; //current Number of points
  Point P[MaxSupported];
  
  Polyloop()
  {

    n=0;
  }
  
  Polyloop(Point PArray[], int Size)
  {
    n=Size;

    for(int i=0;i<Size;i++)
    {
      P[i]=PArray[i];
    }
  }
  

  void AddPoint(Point PP)
  {
    P[n++]=PP;
  }
  
  //Removes from the top - last added
  void RemovePoint()
  {
    --n;
  }
  

  void AddPointAt(Point PP, int i)
  {
    if(i<0 || i>n) return;
    ++n;

    for(int jj=n-1;jj>i;jj--)
    {
        P[jj]=P[jj-1];
    }
    P[i]=PP;
  }
  
  
  void RemovePointAt(int i)
  {
    if(i<0 || i>n-1) return;
    for(int jj=i;jj<n-1;jj++)
    {
        P[jj]=P[jj+1];
    }
    --n;
  }
  
  
  float SignedArea()
  {
    return CalculateSignedArea(P,n);
  }
  
  int IsCW()
  {
    if(SignedArea()>0) return (1); else return false;
  }
  
  void InvertOrientation()
  {
	Point PNew[MaxSupported];
	Point temp; 
    for(int i=0;i<n/2;i++)
	{
		temp.Set(P[i]);
		P[i].Set(P[n-1-i]);
		P[n-1-i].Set(temp);
	}
  }
  
  int next(int j)
  {
    if(j<n-1) return j+1;
    else return 0; 
  }
  
  int prev(int j)
  {
    if(j>0) return j-1;
    else return n-1;
  }
  
  //picked from Prof. Jarek Rosiggnac's library:
  int Contains(Point M)
  {
    int isIn=false; 
    for(int i=1; i<n-1; i++) 
    if (IsPointInTriangle(M, P[0],P[i],P[next(i)])) isIn=!isIn;
    return(isIn);
  }
  void OrientCW()
  {
    if(IsCW()) return; //already CW
    else
    InvertOrientation();    
  }
  
  void OrientCCW()
  {
    if(!IsCW()) return; //already CCW
    else
    InvertOrientation();   
  }
  
  
  Point Project(Point M)
  {
    int v=0; 
    for (int i=1; i<n; i++) 
    if(M.DistanceTo(P[i])<M.DistanceTo(P[v])) v=i;
//    fill(blue); 
//    P[v].DrawCircle(3); 
//    noFill();
    int e=-1;
    float d = M.DistanceTo(P[v]);
    for (int i=0; i<n; i++) if ( M.IsFacingEdge(P[i],P[this->next(i)])  && (M.DistanceToEdge(P[i],P[this->next(i)])<d)) {e=i; d=M.DistanceToEdge(P[e],P[this->next(e)]);};
    if (e!=-1) return(M.ProjectionOnEdge(P[e],P[this->next(e)])); else return(P[v].Clone());
  }
 
     
  //Newer version - to make it consistent with other classes
  void Draw()
  {
    for(int i=0;i<n-1;i++)
    line(P[i].x,P[i].y,P[i+1].x,P[i+1].y);
    line(P[0].x,P[0].y,P[n-1].x,P[n-1].y);    
  }


  //Backward compatibility :)  - Can't delete as other programs are using it
  void Show()
  {
    Draw();
  }


  
};



//************************************************Member Function Definitions***************************************************************
Point Point::TranslateBy(Vector V)
  {
    x+=V.x;
    y+=V.y;
    return *this;
  }

  //translates only by a factor s of vector V
Point Point::TranslateBy(Vector V, float s)
  {
    x+=s*V.x;
    y+=s*V.y;
    return *this;
  }


int Point::IsInCircumcircle(Point A, Point B, Point C)
  {
    Line AB(A,B);
    Line BC(B,C);
    Line Bisector1=AB.PerpendicularThrough(AB.MidPoint());
    Line Bisector2=BC.PerpendicularThrough(BC.MidPoint());
    Intersection I;
    I.returnIntersection(Bisector1,Bisector2);
    if(I.AreLinesParallel) return (1); //As the triangle is actually a straight line, the circumcircle would have infinite radius, in which every point would lie. 
    else//test if the point P lies in the circle with center I.POI and radius r, where r is also calculated
    {
      float r=I.POI.DistanceTo(A); // would be same to any other vertex, as it is radius of the circumcircle. 
      if(I.POI.DistanceTo(*this)<r) return (1); else return false;
    }
     
  }

int Point::IsFacingEdge(Point A, Point B)
  {
    Vector AP(A,*this);
    Vector AB(A,B);
    return (AB.Dot(AP)>0 && AB.Dot(AP)<AB.Dot(AB));
  }
  
  Point Point::ProjectionOnEdge(Point A, Point B)
  {
	  Point res; 
    Vector N(A,B);
	N.Left().Normalize();
    Vector AP(A,*this);
	res=this->Clone().TranslateBy(N.Clone().ScaleBy(-AP.Dot(N)));
    return res; 
  }
  
  
  Point Point::ProjectionOnEdge(Line L)
  {
	  Point res; 
    Vector N(L.a,L.b);
	N.Left().Normalize();
    Vector AP(L.a,*this);
	res=this->Clone().TranslateBy(N.Clone().ScaleBy(-AP.Dot(N)));
    return res;
  }

  float Point::DistanceToEdge(Point A, Point B)
  {
    Vector N(A,B);
	N.Left().Normalize();
    Vector AP(A,*this);
    return abs(AP.Dot(N));  
  }
  
  float Point::DistanceToEdge(Line L)
  {
    Vector N(L.a,L.b);
	N.Left().Normalize();
    Vector AP(L.a,*this);
    return abs(AP.Dot(N));
  }


  Vector Point::VectorTo(Point B)
  {
    return *(new  Vector(*this, B));
  }



  Triangle::Triangle(Line ln1, Line ln2, Line ln3)
  {
    Intersection Int1;
    Intersection Int2;
    Intersection Int3;
    this->T1=Int1.returnIntersection(ln1, ln2);
    this->T2=Int2.returnIntersection(ln2, ln3);
    this->T2=Int3.returnIntersection(ln3, ln1);
	Line L1;
	Line L2;
	Line L3;
    L1.a=T1;
    L1.b=T2;
    L2.a=T2;
    L2.b=T3;
    L3.a=T3;
    L3.b=T1;

  }

//************************************************COMMON FUNCTIONS***************************************************************





float distance(Point A, Point B)
{
  return sqrt(sq(A.x-B.x)+sq(A.y-B.y));

}

float distance(float x1,float y1,float x2,float y2)
{
  return sqrt(sq(x1-x2)+sq(y1-y2));
}

float distance(Line L)
{
  return distance(L.a,L.b);
}


int IsPointInTriangle(float x,float y,float Tx1,float Ty1,float Tx2,float Ty2,float Tx3,float Ty3)
{
  if((((Ty1-Ty2)*(x-Tx1)+(Tx2-Tx1)*(y-Ty1))>0 && ((Ty2-Ty3)*(x-Tx2)+(Tx3-Tx2)*(y-Ty2))>0 && ((Ty3-Ty1)*(x-Tx3)+(Tx1-Tx3)*(y-Ty3))>0) ||(((Ty1-Ty2)*(x-Tx1)+(Tx2-Tx1)*(y-Ty1))<0 && ((Ty2-Ty3)*(x-Tx2)+(Tx3-Tx2)*(y-Ty2))<0 && ((Ty3-Ty1)*(x-Tx3)+(Tx1-Tx3)*(y-Ty3))<0))
    return (1);

  return false;

}


int IsPointInTriangle(Point P,Point T1,Point T2,Point T3)
{
  return IsPointInTriangle(P.x, P.y, T1.x, T1.y, T2.x, T2.y, T3.x, T3.y);

}


//Only the Length number of items are checked
Point GetClosestPoint(Point P, Point PArray[], int Length)
{
  return GetClosestPoint(P, PArray, 0,Length-1);
}


Point GetClosestPoint(Point P, Point PArray[], int start, int end)
{
  Point res=PArray[start];
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


float AngleToDraw(Point P)
{
  return AngleToDraw(P.y,P.x);
}

float AngleToDraw(Vector V)
{
  return AngleToDraw(V.y,V.x);  
}

//Essentially returns the Anticlockwise Angle in paper from the x-axis, but in computer screen as y axis is down, returns clockwise. 
float AngleToDraw(float y,float x)
{
  float AbsVal=atan(abs(y)/abs(x));
  float res;

  if(x>0)
  {
    if(y>0)
    {
      res=AbsVal;
    }
    else
    {
      res=2*PI-AbsVal;
    }
  }
  else
  {
    if(y>0)
    {
      res=PI-AbsVal;
    }
    else
    {
      res=PI+AbsVal;
    }
  }

  return res;

}



float CalculateSignedArea(Point P[], int n)
{
  
  int i;
  Triangle T;
  float TotalArea=0;

  //check if the Array has something at all, and if it has atleast 3 points
  if(P==NULL || n<3) return 0; 

  for(i=0;i<n-1;i++)
  {
    T.SetVertices(P[i],P[i+1],Origin);

    TotalArea+=T.SignedArea();
  }
  T.SetVertices(P[i], P[0], Origin);
  TotalArea+=T.SignedArea();
  return TotalArea;
}

}