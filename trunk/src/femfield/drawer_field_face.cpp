/*
DelFEM (Finite Element Analysis)
Copyright (C) 2009  Nobuyuki Umetani    n.umetani@gmail.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

////////////////////////////////////////////////////////////////
// DrawerField.cpp : 場可視化クラス(DrawerField)の実装
////////////////////////////////////////////////////////////////

#if defined(__VISUALC__)
    #pragma warning ( disable : 4786 )
#endif

#if defined(_WIN32)
#  include <windows.h>
#if defined(__VISUALC__)
#  pragma comment (lib, "winmm.lib")      /* link with Windows MultiMedia lib */
#  pragma comment (lib, "opengl32.lib")  /* link with Microsoft OpenGL lib */
#  pragma comment (lib, "glu32.lib")     /* link with Microsoft OpenGL Utility lib */
#endif
#endif  /* _WIN32 */

#include <assert.h>
#include <iostream>
#include <vector>
#include <stdio.h>


#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

#include "delfem/drawer_field_face.h"
#include "delfem/uglyfont.h"
#include "delfem/elem_ary.h"
#include "delfem/field.h"
#include "delfem/drawer.h"
#include "delfem/vector3d.h"

using namespace Fem::Field::View;
using namespace Fem::Field;


namespace Fem{
namespace Field{
namespace View{

// 最大値(max)から最小値(min)のデータを青から赤であらわしたRGBを返す
static void GetColor( float color[], const double& val, const double& max, const double& min )
{
	const double d = ( 2.0 * ( val - min ) ) / (max - min)  - 1.0;
	if(d > 0.5){
		color[0] = 1;
		color[1] = static_cast<float>(2 - 2*d);
		color[2] = 0;
	}
	else if(d > 0.0 && d <= 0.5){
		color[0] = static_cast<float>(-4*d*d + 4*d);
		color[1] = 1;
		color[2] = 0;
	}
	else if(d <= 0.0 && d > -0.5){
		color[0] = 0;
		color[1] = 1;
		color[2] = static_cast<float>(-4*d*d - 4*d);
	}
	else if(d <= -0.5){
		color[0] = 0;
		color[1] = static_cast<float>(2 + 2*d);
		color[2] = 1;
	}
}

}
}
}


CDrawerFace::CDrawerFace()
{
	pColorArray = 0;
	is_min_max_set = false;
}

CDrawerFace::CDrawerFace(const unsigned int id_field, bool isnt_value_disp, 
						 const Fem::Field::CFieldWorld& world, unsigned int id_field_color)
{
	pColorArray = 0;
	is_min_max_set = false;
	this->Set( id_field, world, isnt_value_disp, id_field_color);
}

CDrawerFace::CDrawerFace(const unsigned int id_field, bool isnt_value_disp, 
						 const Fem::Field::CFieldWorld& world, 
						 unsigned int id_field_color, double min_val, double max_val )
{
	pColorArray = 0;
	if( min_val < max_val )
	{
		is_min_max_set = true;
		this->min_val = min_val;
		this->max_val = max_val;
	}
	else{
		is_min_max_set = false;
	}
	this->Set( id_field, world, isnt_value_disp, id_field_color);
}

CDrawerFace::~CDrawerFace()
{
	if( pColorArray != 0 ){ delete[] pColorArray; }
	for(unsigned int i=0;i<this->m_apIndexArrayElem.size();i++){
		delete this->m_apIndexArrayElem[i];
	}
}

void CDrawerFace::Draw() const 
{
	if( m_vertex_ary.NDim() == 2 ){	// cannot see the opposite side
		::glEnable(GL_CULL_FACE);
		::glCullFace(GL_BACK);
	}
	else{ ::glDisable(GL_CULL_FACE); }

	if( this->pColorArray == 0 ){
		glLineWidth(3);
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(m_vertex_ary.NDim(),GL_DOUBLE,0,m_vertex_ary.pVertexArray);
		for(unsigned int idp=0;idp<this->m_apIndexArrayElem.size();idp++){ 
			View::CIndexArrayElem* pIndexArray = this->m_apIndexArrayElem[idp];
			if( pIndexArray->GetElemDim() == 2 ){	// 線の描画
				::glColor3d(0.0,0.0,0.0);
				::glLineWidth(3);
			}
			if( pIndexArray->GetElemDim() == 3 ){	// 面の描画
				 ::glColor3d(0.8,0.8,0.8);
			}
			this->m_apIndexArrayElem[idp]->DrawElements(); 
		}
		::glDisableClientState(GL_VERTEX_ARRAY);
	}
	else{
        ::glShadeModel(GL_SMOOTH);
		::glEnableClientState(GL_VERTEX_ARRAY);
		::glVertexPointer(m_vertex_ary.NDim(),GL_DOUBLE,0,m_vertex_ary.pVertexArray);
		::glEnableClientState(GL_COLOR_ARRAY);
		::glColorPointer(4,GL_FLOAT,0,pColorArray);
		for(unsigned int idp=0;idp<this->m_apIndexArrayElem.size();idp++){ 
			this->m_apIndexArrayElem[idp]->DrawElements(); 
		}
		::glDisableClientState(GL_COLOR_ARRAY);
		::glDisableClientState(GL_VERTEX_ARRAY);
	}

    ::glShadeModel(GL_SMOOTH);
	::glDisable(GL_CULL_FACE);
	::glLineWidth(1);
	::glColor3d(0,0,0);
	
    // Get View Port
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
	double asp = (viewport[2]+1.0)/(viewport[3]+1.0);
	
	::glMatrixMode(GL_PROJECTION);
	::glPushMatrix();
	::glLoadIdentity();
	::glOrtho(-asp,asp, -1,1, -1,1);
	
	::glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	::glLoadIdentity();
	::glTranslated((asp-9*0.03)-0.05, 
		(1-0.05*10.5*1.7)-0.05,
		0.3);
	
	////////////////
	double interval_n = 1.7;
	const unsigned int ndiv_c = 20;
	::glScaled(0.03,0.05,1.0);
	::glBegin(GL_QUADS);
	for(unsigned int i=0;i<ndiv_c;i++){
		{
			::glVertex2d(-3,interval_n*(10.0/ndiv_c)*i  );
			::glVertex2d(-0,interval_n*(10.0/ndiv_c)*i  );
			float color0[3];
			const double val0 = min_val + (max_val-min_val)*(i+1)/ndiv_c;
			GetColor(color0, val0, max_val, min_val );
			::glColor3fv(color0);
		}
		{
			::glVertex2d(-0,interval_n*(10.0/ndiv_c)*(i+1));
			::glVertex2d(-3,interval_n*(10.0/ndiv_c)*(i+1));
			float color1[3];
			const double val1 = min_val + (max_val-min_val)*(i+1)/ndiv_c;
			GetColor(color1, val1, max_val, min_val );
			::glColor3fv(color1);
		}
	}
	::glEnd();
	////////////////
	glColor3f(0,0,0);
	::glTranslated(0,-0.5,0);
	const unsigned int ndiv_n = 10;
	for(unsigned int i=0;i<ndiv_n+1;i++){
		double val = (max_val-min_val)*i/ndiv_n + min_val;
		char str1[32];
		sprintf(str1,"% 5.1e",val);
		::YsDrawUglyFont(str1,false,false);
		::glTranslated(0,+interval_n,0);
	}
	::glMatrixMode(GL_PROJECTION);
	::glPopMatrix();
	::glMatrixMode(GL_MODELVIEW);
	::glPopMatrix();
}


bool CDrawerFace::Update(const Fem::Field::CFieldWorld& world)
{
	const Fem::Field::CField& field = world.GetField(m_id_field);
	// 頂点配列の設定
	unsigned int id_na_c_co = field.GetNodeSegInNodeAry(CORNER).id_na_co;
	assert( id_na_c_co != 0 );
	const Fem::Field::CNodeAry& na_c_co = world.GetNA(id_na_c_co);
	const Fem::Field::CNodeAry::CNodeSeg& ns_c_co = field.GetNodeSeg(CORNER,false,world);
	const unsigned int ndim = ns_c_co.GetLength();
	const unsigned int npoin_co = na_c_co.Size();
	unsigned int npoin;	// 表示する節点の数を計算してAssertionする
	{
		if( m_is_draw_nsv ){
			unsigned int id_na_c_val = field.GetNodeSegInNodeAry(CORNER).id_na_va;
			assert( world.IsIdNA(id_na_c_val) );
			const Fem::Field::CNodeAry& na_c_val = world.GetNA(id_na_c_val);
			npoin = na_c_val.Size();
		}
		else{ npoin = npoin_co; }
	}
	assert( m_vertex_ary.NPoin() == npoin );

	if( !m_isnt_value_disp ){	// 変位が反映される場合
		unsigned int id_na_c_val = field.GetNodeSegInNodeAry(CORNER).id_na_va;
		assert( world.IsIdNA(id_na_c_val) );
		const Fem::Field::CNodeAry& na_c_val = world.GetNA(id_na_c_val);
		const Fem::Field::CNodeAry::CNodeSeg& ns_c_val = field.GetNodeSeg(CORNER,true,world,VALUE|VELOCITY|ACCELERATION);
		if( ndim == 2 && (field.GetFieldType()==SCALAR||field.GetFieldType()==ZSCALAR) )	// 垂直方向の変位として捉える
		{
			assert( m_vertex_ary.NDim() == 3 );
			double coord[2], value[2];
			for(unsigned int ipoin=0;ipoin<npoin;ipoin++){
				unsigned int ipoin_co = field.GetMapVal2Co(ipoin);
				assert( ipoin_co < npoin_co );
				ns_c_val.GetValue(ipoin,value);
				ns_c_co.GetValue(ipoin_co,coord);
				this->m_vertex_ary.pVertexArray[ipoin*3+0] = coord[0];
				this->m_vertex_ary.pVertexArray[ipoin*3+1] = coord[1];
				this->m_vertex_ary.pVertexArray[ipoin*3+2] = value[0];
			}
		}
		else{
			assert( m_vertex_ary.NDim() == ndim );
			assert( ndim == ns_c_val.GetLength() ); // Coordの次元とValueの次元が合ってなければならない
			assert( npoin_co == na_c_co.Size() );
			assert( na_c_val.Size() == npoin );
			assert( ndim <= 3 );
			double coord[3], value[3];
			for(unsigned int ipoin=0;ipoin<npoin;ipoin++){
				unsigned int ipoin_co = field.GetMapVal2Co(ipoin);
				assert( ipoin_co < npoin_co );
				ns_c_val.GetValue(ipoin,value);
				ns_c_co.GetValue(ipoin_co,coord);
				for(unsigned int idim=0;idim<ndim;idim++){
					this->m_vertex_ary.pVertexArray[ipoin*ndim+idim] = coord[idim]+value[idim];
				}
			}
		}
	}
	else{		
		assert( m_vertex_ary.NDim() == ndim );
		for(unsigned int ipoin=0;ipoin<npoin;ipoin++){
			unsigned int ipoin_co = field.GetMapVal2Co(ipoin);
			assert( ipoin_co < npoin_co );
			double* pval = &this->m_vertex_ary.pVertexArray[ipoin*ndim];
			ns_c_co.GetValue(ipoin_co,pval);
		}
	}

	////////////////////////////////////////////////
	// 色をつくる
	
	if( world.IsIdField(id_field_val) )
	{
		const Fem::Field::CField& field_val = world.GetField(id_field_val);
//		unsigned int id_na_c_co = field_val.GetNodeSegInNodeAry(CORNER).id_na_co;
		unsigned int id_na_c_val = field_val.GetNodeSegInNodeAry(CORNER).id_na_va;
		unsigned int id_na_b_val = field_val.GetNodeSegInNodeAry(BUBBLE).id_na_va;
		if(      world.IsIdNA(id_na_c_val) ){
//			std::cout << "Set Color C" << std::endl;
			const CField::CNodeSegInNodeAry& nsna_c = field_val.GetNodeSegInNodeAry(CORNER);
			const unsigned int id_na_c_val = nsna_c.id_na_va;
			assert( id_na_c_val != 0 );
			assert( world.IsIdNA(id_na_c_val) );
			const Fem::Field::CNodeAry& na = world.GetNA(id_na_c_val);
			const unsigned int npoin = this->m_vertex_ary.NPoin();
			assert( npoin == na.Size() );
			Fem::Field::FIELD_DERIVATION_TYPE fdt;
			unsigned int id_ns_v;
			{	// できるだけValueの値を描画する
				if(      nsna_c.id_ns_va != 0 ){ id_ns_v = nsna_c.id_ns_va; fdt = VALUE;        }
				else if( nsna_c.id_ns_ve != 0 ){ id_ns_v = nsna_c.id_ns_ve; fdt = VELOCITY;     }
				else if( nsna_c.id_ns_ac != 0 ){ id_ns_v = nsna_c.id_ns_ac; fdt = ACCELERATION; }
				else{ assert(0); }
			}
			assert( na.IsSegID(id_ns_v) );
			const CNodeAry::CNodeSeg& ns_v = na.GetSeg(id_ns_v);
			////////////////
			if( !is_min_max_set ){	// 値の最大値最小値を求める
				field_val.GetMinMaxValue(min_val,max_val,world,0,fdt);
			}
			{	// Color配列に値をセット
				double val[10];
				if( pColorArray == 0 ){ pColorArray = new float [npoin*4]; }
				for(unsigned int ipoin=0;ipoin<npoin;ipoin++){
					ns_v.GetValue(ipoin,val);
					Fem::Field::View::GetColor(&pColorArray[ipoin*4],val[0],max_val,min_val);
					pColorArray[ipoin*4+3] = 0.0f;
				}
			}
		}
		else if( world.IsIdNA(id_na_b_val) ){
//			std::cout << "Set Color B" << std::endl;
			unsigned int id_na_v, id_ns_v;
			{
				const CField::CNodeSegInNodeAry& nans_b = field_val.GetNodeSegInNodeAry(BUBBLE);
				id_na_v = nans_b.id_na_va;
				assert( world.IsIdNA(id_na_v) );
				{
					if(      nans_b.id_ns_va != 0 ) id_ns_v = nans_b.id_ns_va;
					else if( nans_b.id_ns_ve != 0 ) id_ns_v = nans_b.id_ns_ve;
					else if( nans_b.id_ns_ac != 0 ) id_ns_v = nans_b.id_ns_ac;
					else{ assert(0); }
				}
			}
			assert( id_na_v != 0 );
			const Fem::Field::CNodeAry& na = world.GetNA(id_na_v);
			assert( na.IsSegID(id_ns_v) );
			if( !is_min_max_set ){	// 値の最大値最小値を求める
				assert( na.IsSegID(id_ns_v) );
				const CNodeAry::CNodeSeg& ns_v = na.GetSeg(id_ns_v);
				const unsigned int npoin = na.Size();
				double val;
				ns_v.GetValue(0,&val);
				max_val = val; min_val = val;
				for(unsigned int ipoin=1;ipoin<npoin;ipoin++){
					ns_v.GetValue(ipoin,&val);
					max_val = ( val > max_val ) ? val : max_val;
					min_val = ( val < min_val ) ? val : min_val;
				}
//				std::cout << min_val << " " << max_val << std::endl;
			}
			for(unsigned int idp=0;idp<this->m_apIndexArrayElem.size();idp++){ 
				View::CIndexArrayElem* pIA = this->m_apIndexArrayElem[idp];
				unsigned int id_ea = pIA->GetIdEA();
				unsigned int id_es_v = field_val.GetIdElemSeg(id_ea,BUBBLE,true,world);
				pIA->SetColor(id_es_v,id_ns_v,world,min_val,max_val);
			}
		}
	}


	return true;
}

bool CDrawerFace::Set(unsigned int id_field, const Fem::Field::CFieldWorld& world, bool isnt_value_disp,
					  unsigned int id_field_val)
{
	if( !world.IsIdField(id_field) ){
		assert(0);
		return false;
	}
	this->m_id_field = id_field;
	this->id_field_val = id_field_val;
	this->m_isnt_value_disp = isnt_value_disp;

	const Fem::Field::CField& field = world.GetField(id_field);

	// 頂点配列の設定
	unsigned int id_na_c_co = field.GetNodeSegInNodeAry(CORNER).id_na_co;
	unsigned int id_na_c_val = field.GetNodeSegInNodeAry(CORNER).id_na_va;
	////////////////////////////////
	// ns_vを描画するかどうかを決める
	if( id_na_c_val == 0 ){ 
		this->m_is_draw_nsv = false;	// CoordのNSを描画 
		this->m_isnt_value_disp = true;	// 変位には対応しない
	}
	else{ this->m_is_draw_nsv = true;}
	////////////////
	assert( field.IsNodeSeg(CORNER,false,world,VALUE) );
	unsigned int ndim_field = field.GetNDimCoord();
	////////////////
	unsigned int npoin;
	if( m_is_draw_nsv ){
		assert( world.IsIdNA(id_na_c_val) );
		const Fem::Field::CNodeAry& na_c_val = world.GetNA(id_na_c_val);
		npoin = na_c_val.Size();
	}
	else{ 
		assert( id_na_c_co != 0 );
		const Fem::Field::CNodeAry& na_c_co = world.GetNA(id_na_c_co);
		npoin = na_c_co.Size(); 
	}
	////////////////
	unsigned int ndim_draw;
	if( this->m_isnt_value_disp == false && ndim_field == 2 && (field.GetFieldType()==SCALAR||field.GetFieldType()==ZSCALAR) ){
		ndim_draw = 3;
	}
	else{ ndim_draw = ndim_field; }
	this->m_vertex_ary.SetSize(npoin,ndim_draw);
	////////////////
	if(      ndim_draw  == 2 ){ sutable_rot_mode = 1; }
	else if( ndim_field == 3 ){ sutable_rot_mode = 3; }
	else{ sutable_rot_mode = 2; }
	CDrawerFace::Update(world);

	////////////////////////////////
	// 要素配列の設定

	const std::vector<unsigned int>& aIdEA = field.GetAry_IdElemAry();
	for(unsigned int iiea=0;iiea<aIdEA.size();iiea++){
		const unsigned int id_ea = aIdEA[iiea];
		assert( world.IsIdEA(id_ea) );
		unsigned int id_es_c;
		if( m_is_draw_nsv ){ id_es_c = field.GetIdElemSeg(id_ea,CORNER,true, world); }
		else{                id_es_c = field.GetIdElemSeg(id_ea,CORNER,false,world); }
		assert( id_es_c != 0 );
//		const CElemAry& ea = world.GetEA(id_ea);
//		Fem::Field::ELEM_TYPE elem_type = ea.ElemType();
		this->m_apIndexArrayElem.push_back( new CIndexArrayElem(id_ea,id_es_c,world) );
	}

	////////////////////////////////
	// 色の設定

	if( world.IsIdField(id_field_val) ){
		const Fem::Field::CField& field_val = world.GetField(id_field_val);
//		unsigned int id_na_c_co  = field_val.GetNodeSegInNodeAry(CORNER).id_na_co;
		unsigned int id_na_c_val = field_val.GetNodeSegInNodeAry(CORNER).id_na_va;
		unsigned int id_na_b_val = field_val.GetNodeSegInNodeAry(BUBBLE).id_na_va;
		if(      world.IsIdNA(id_na_c_val) ){
//			std::cout << "Color C" << std::endl;
			if( pColorArray != 0 ){ delete[] pColorArray;  pColorArray=0; }
		}
		else if( world.IsIdNA(id_na_b_val) ){
//			std::cout << "Color B" << std::endl;
		}
	}
	this->Update(world);

	return true;
}
