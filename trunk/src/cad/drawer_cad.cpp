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
// DrawerCad.cpp : CADモデル描画クラス(CDrawerCad)の実装
// このクラスは何が起きても絶対落ちないように実装すること．
// assertionも原則しないこと
////////////////////////////////////////////////////////////////

#define for if(0);else for

#if defined(_WIN32)
#include <windows.h>
#if defined(__VISUALC__)
#pragma comment (lib, "winmm.lib")     /* link with Windows MultiMedia lib */
#pragma comment (lib, "opengl32.lib")  /* link with Microsoft OpenGL lib */
#pragma comment (lib, "glu32.lib")     /* link with Microsoft OpenGL Utility lib */
#endif
#endif  /* _WIN32 */


#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <assert.h>
#include <iostream>

#include "delfem/drawer_cad.h"
#include "delfem/cad2d_interface.h"

#include "delfem/mesher2d.h"

using namespace Cad::View;
using namespace Com;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


void CDrawerRubberBand::Draw() const
{
	::glLineWidth(2);
	::glColor3d(0.0,0.0,0.0);
	::glBegin(GL_LINES);
	if( imode == 0 ){
		::glVertex3d(initial.x, initial.y, initial.z);
		::glVertex3d(mouse.x, mouse.y, mouse.z);
	}
	else if( imode == 1 ){
		for(unsigned int iline=0;iline<fix.size();iline++){
			::glVertex3d(fix[iline].x, fix[iline].y, fix[iline].z);
			::glVertex3d(mouse.x, mouse.y, mouse.z);
		}
	}
	else if( imode == 2 ){
		assert( fix.size() == 2 );
		::glVertex3d(
			fix[0].x + mouse.x - initial.x,  
			fix[0].y + mouse.y - initial.y,  
			fix[0].z + mouse.z - initial.z );
		::glVertex3d(
			fix[1].x + mouse.x - initial.x,
			fix[1].y + mouse.y - initial.y, 
			fix[1].z + mouse.z - initial.z );
		for(unsigned int iline=0;iline<fix_stat.size();iline++){
			::glVertex3d(
				fix_stat[iline].second.x, 
				fix_stat[iline].second.y, 
				fix_stat[iline].second.z);
			unsigned int ifix = fix_stat[iline].first;
			::glVertex3d(
				fix[ifix].x + mouse.x - initial.x, 
				fix[ifix].y + mouse.y - initial.y, 
				fix[ifix].z + mouse.z - initial.z );
		}
	}
	::glEnd();
}

Com::CBoundingBox CDrawerRubberBand::GetBoundingBox( double rot[] ) const
{
	////////////////
	double x_max,x_min,  y_max,y_min,  z_max,z_min;
	{
		const double x1 = mouse.x; const double y1 = mouse.y; const double z1 = mouse.z;
		const double x2 = x1*rot[0]+y1*rot[1]+z1*rot[2];
		const double y2 = x1*rot[3]+y1*rot[4]+z1*rot[5];
		const double z2 = x1*rot[6]+y1*rot[7]+z1*rot[8];
		x_max = x2;  x_min = x2;
		y_max = y2;  y_min = y2;
		z_max = z2;  z_min = z2;
	}
	for(unsigned int iline=0;iline<fix.size();iline++){
		const double x1 = fix[iline].x; const double y1 = fix[iline].y; const double z1 = fix[iline].z;
		const double x2 = x1*rot[0]+y1*rot[1]+z1*rot[2];
		const double y2 = x1*rot[3]+y1*rot[4]+z1*rot[5];
		const double z2 = x1*rot[6]+y1*rot[7]+z1*rot[8];
		x_max = ( x2 > x_max ) ? x2 : x_max;  x_min = ( x2 < x_min ) ? x2 : x_min;			
		y_max = ( y2 > y_max ) ? y2 : y_max;  y_min = ( y2 < y_min ) ? y2 : y_min;			
		z_max = ( z2 > z_max ) ? z2 : z_max;  z_min = ( z2 < z_min ) ? z2 : z_min;			
	}
	double x_size = x_max - x_min;
	double y_size = y_max - y_min;
	double z_size = z_max - z_min;
	double x_cent1 = ( x_max + x_min )*0.5;
	double y_cent1 = ( y_max + y_min )*0.5;
	double z_cent1 = ( z_max + z_min )*0.5;
//	double x_cent = x_cent1*rot[0]+y_cent1*rot[3]+z_cent1*rot[6];
//	double y_cent = x_cent1*rot[1]+y_cent1*rot[4]+z_cent1*rot[7];
//  double z_cent = x_cent1*rot[2]+y_cent1*rot[5]+z_cent1*rot[8];
	return CBoundingBox(
		x_cent1-x_size*0.5,x_cent1+x_size*0.5,
		y_cent1-y_size*0.5,y_cent1+y_size*0.5,
		z_cent1-z_size*0.5,z_cent1+z_size*0.5 );
}

CDrawerRubberBand::CDrawerRubberBand(const Cad::ICad2D& cad, unsigned int id_v)
{
	this->sutable_rot_mode = 1;	
	std::auto_ptr<ICad2D::CItrVertex> itrv = cad.GetItrVertex(id_v);
	for(;!itrv->IsEnd();(*itrv)++){
		unsigned int id_e; bool is_same_dir;
		itrv->GetIdEdge_Behind(id_e,is_same_dir);
		unsigned int id_vs, id_ve;
		cad.GetIdVertex_Edge(id_vs,id_ve,id_e);
		const unsigned int id_v1 = (is_same_dir) ? id_ve : id_vs;
		assert( ((is_same_dir) ? id_vs : id_ve) == id_v );
        assert( cad.IsElemID(Cad::VERTEX,id_v1) );
		const Com::CVector2D& vec2d = cad.GetVertexCoord(id_v1);
		fix.push_back( CVector3D(vec2d.x, vec2d.y, 0.0) );
	}
	imode = 1;
}

CDrawerRubberBand::CDrawerRubberBand(const Cad::ICad2D& cad,
                                     unsigned int id_e, const Com::CVector3D& initial)
{
	this->sutable_rot_mode = 1;	
	if( !cad.IsElemID(Cad::EDGE,id_e) ) return;
	this->initial = initial;
	unsigned int id_vs, id_ve;
	cad.GetIdVertex_Edge(id_vs,id_ve,id_e);
	fix.clear();
	{
		{
            assert( cad.IsElemID(Cad::VERTEX,id_vs) );
			const Com::CVector2D& vec2d = cad.GetVertexCoord(id_vs);
			fix.push_back( CVector3D(vec2d.x,vec2d.y,0) );
		}
		{
			std::auto_ptr<ICad2D::CItrVertex> itrv = cad.GetItrVertex(id_vs);
			for(;!itrv->IsEnd();(*itrv)++){
				unsigned int id_e0; bool is_same_dir0;
				itrv->GetIdEdge_Behind(id_e0,is_same_dir0);
				if( id_e0 == id_e ) continue;
				unsigned int id_vs0, id_ve0;
				cad.GetIdVertex_Edge(id_vs0,id_ve0,id_e0);
				const unsigned int id_v1 = (is_same_dir0) ? id_ve0 : id_vs0;
				assert( ((is_same_dir0) ? id_vs0 : id_ve0) == id_vs );
				assert( cad.IsElemID(Cad::VERTEX,id_v1) );
				const Com::CVector2D& vec2d = cad.GetVertexCoord(id_v1);
				fix_stat.push_back( std::make_pair(0,Com::CVector3D(vec2d.x, vec2d.y, 0.0)) );
			}
		}
		{
            assert( cad.IsElemID(Cad::VERTEX,id_ve) );
			const Com::CVector2D& vec2d = cad.GetVertexCoord(id_ve);
			fix.push_back( CVector3D(vec2d.x,vec2d.y,0) );
		}
		{
			std::auto_ptr<ICad2D::CItrVertex> itrv = cad.GetItrVertex(id_ve);
			for(;!itrv->IsEnd();(*itrv)++){
				unsigned int id_e0; bool is_same_dir0;
				itrv->GetIdEdge_Behind(id_e0,is_same_dir0);
				if( id_e0 == id_e ) continue;
				unsigned int id_vs0, id_ve0;
				cad.GetIdVertex_Edge(id_vs0,id_ve0,id_e);
				const unsigned int id_v1 = (is_same_dir0) ? id_ve0 : id_vs0;
				assert( ((is_same_dir0) ? id_vs0 : id_ve0) == id_ve );
				assert( cad.IsElemID(Cad::VERTEX,id_v1) );
				const Com::CVector2D& vec2d = cad.GetVertexCoord(id_v1);
				fix_stat.push_back( std::make_pair(1,Com::CVector3D(vec2d.x, vec2d.y, 0.0)) );
			}
		}
	}
	imode = 2;
}


////////////////////////////////////////////////////////////////

bool CDrawer_Cad2D::CDrawPart::Set(const Msh::CTriAry2D& TriAry)
{
	this->id_cad = TriAry.id_l_cad;	assert( id_cad != 0 );
	this->id_msh = TriAry.id;		assert( id_msh != 0 );
	this->itype  = Cad::LOOP;
	////////////////
	npoel = 3;
	nelem = TriAry.m_aTri.size();
	if( pIndexArray != 0 ){ delete[] pIndexArray; }
	pIndexArray = new unsigned int [nelem*npoel];
	for(unsigned int ielem=0;ielem<nelem;ielem++){
		for(unsigned int ipoel=0;ipoel<npoel;ipoel++){
			pIndexArray[ielem*npoel+ipoel] = TriAry.m_aTri[ielem].v[ipoel];
		}
	}
    ////////////////
    color[0]=0.8;   color[1]=0.8;   color[2]=0.8;
	return true;
}

bool CDrawer_Cad2D::CDrawPart::Set(const Msh::CBarAry& BarAry)
{
	this->id_cad = BarAry.id_e_cad;	assert( id_cad != 0 );
	this->id_msh = BarAry.id;		assert( id_msh != 0 );
	this->itype  = Cad::EDGE;
	////////////////
	npoel = 2;
	nelem = BarAry.m_aBar.size();
	if( pIndexArray != 0 ){ delete[] pIndexArray; }
	pIndexArray = new unsigned int [nelem*npoel];
	for(unsigned int ielem=0;ielem<nelem;ielem++){
		for(unsigned int ipoel=0;ipoel<npoel;ipoel++){
			pIndexArray[ielem*npoel+ipoel] = BarAry.m_aBar[ielem].v[ipoel];
		}
	}
    ////////////////
    color[0]=0.0;   color[1]=0.0;   color[2]=0.0;
	return true;
}

void CDrawer_Cad2D::CDrawPart::DrawElements() const
{
//	if( !this->is_show ) return;    // セレクションに必要
	if( npoel == 2 ){ 
		::glDrawElements(GL_LINES    ,nelem*npoel,GL_UNSIGNED_INT,pIndexArray); 
		return;
	}
	else if( npoel == 3 ){ 
		::glDrawElements(GL_TRIANGLES,nelem*npoel,GL_UNSIGNED_INT,pIndexArray); 
		return;
	}
	// assert( npoel == 0 );
}

////////////////////////////////////////////////////////////////

bool CDrawer_Cad2D::UpdateCAD_TopologyGeometry(const Cad::ICad2D &cad_2d)
{
	this->sutable_rot_mode = 1;	
    //! 今までのDrawerPartの配列を一端バッファにコピーして，必要な物だけを戻す
    std::vector<CDrawPart*> indAry_old = m_apIndexAry;
    for(unsigned int idp=0;idp<indAry_old.size();idp++){
        indAry_old[idp]->id_msh = 0;
        indAry_old[idp]->is_selected = false;
    }
    m_apIndexAry.clear();
    m_aIndexVertex.clear();

	Msh::CMesher2D mesh;
	mesh.Tesselation(cad_2d);

	int ilayer_min, ilayer_max;
	{
		const std::vector<unsigned int>& aIdL = cad_2d.GetAryElemID(Cad::LOOP);
		{
			assert( aIdL.size() > 0 );
			unsigned int id_l0 = aIdL[0];
			ilayer_min = cad_2d.GetLayer_Loop(id_l0);
			ilayer_max = ilayer_min;
		}
		for(unsigned int il=0;il<aIdL.size();il++){
			unsigned int id_l = aIdL[il];
			int ilayer = cad_2d.GetLayer_Loop(id_l);
			ilayer_min = ( ilayer < ilayer_min ) ? ilayer : ilayer_min;
			ilayer_max = ( ilayer > ilayer_max ) ? ilayer : ilayer_max;
		}
	}
	double layer_height = 1.0/(ilayer_max-ilayer_min+1); 

	{	// 面をセット
		const std::vector<Msh::CTriAry2D>& aTriAry = mesh.GetTriArySet();
        for(unsigned int ita=0;ita<aTriAry.size();ita++){
            const unsigned int id_l = aTriAry[ita].id_l_cad;
			double height = 0;
			{
				unsigned int ilayer = cad_2d.GetLayer_Loop(id_l);
				height = (ilayer-ilayer_min)*layer_height;
			}
            unsigned int idp0 = 0;
            for(;idp0<indAry_old.size();idp0++){
                if(    indAry_old[idp0]->itype == Cad::LOOP
                    && indAry_old[idp0]->id_cad == id_l )
                {
                    indAry_old[idp0]->Set( aTriAry[ita] );
                    indAry_old[idp0]->SetHeight(height);
                    cad_2d.GetColor_Loop(id_l,indAry_old[idp0]->color);
                    this->m_apIndexAry.push_back( indAry_old[idp0] );
                    break;
                }
            }
            if( idp0 == indAry_old.size() ){
                CDrawPart* dp = new CDrawPart;
                dp->Set( aTriAry[ita] );
                dp->SetHeight( height );
                cad_2d.GetColor_Loop(id_l,dp->color);
                this->m_apIndexAry.push_back( dp );
            }
		}
	}

	{	// 辺をセット
		const std::vector<Msh::CBarAry>& aBarAry = mesh.GetBarArySet();
        for(unsigned int ibar=0;ibar<aBarAry.size();ibar++){
            const unsigned int id_e = aBarAry[ibar].id_e_cad;
			double height = 0;
			{
				unsigned int id_l_l, id_l_r;
				cad_2d.GetIdLoop_Edge(id_l_l,id_l_r,id_e);
				int ilayer = ilayer_min;
				if( cad_2d.IsElemID(Cad::LOOP,id_l_l) ){
					const int ilayer_l = cad_2d.GetLayer_Loop(id_l_l);
					ilayer =  ( ilayer_l > ilayer )	? ilayer_l : ilayer;
				}
				if( cad_2d.IsElemID(Cad::LOOP,id_l_r) ){
					const int ilayer_r = cad_2d.GetLayer_Loop(id_l_r);
					ilayer =  ( ilayer_r > ilayer )	? ilayer_r : ilayer;
				}
				height += (ilayer-ilayer_min+0.01)*layer_height;
			}
            unsigned int idp0 = 0;
            for(;idp0<indAry_old.size();idp0++){
                if(    indAry_old[idp0]->itype  == Cad::EDGE
                    && indAry_old[idp0]->id_cad == id_e )
                {
                    indAry_old[idp0]->Set( aBarAry[ibar] );
                    indAry_old[idp0]->SetHeight( height );
                    this->m_apIndexAry.push_back( indAry_old[idp0] );
                    break;
                }
            }
            if( idp0 == indAry_old.size() ){
                CDrawPart* dp = new CDrawPart;
                dp->Set( aBarAry[ibar] );	
                dp->SetHeight( height );
                this->m_apIndexAry.push_back( dp );
            }
		}
	}

    for(unsigned int idp=0;idp<indAry_old.size();idp++){
        if( indAry_old[idp]->id_msh == 0 ){
            delete indAry_old[idp];
        }
    }
    indAry_old.clear();

	{	// 頂点をセット
		const std::vector<Msh::SVertex>& aVertex = mesh.GetVertexAry();
		assert( aVertex.size() > 0 );
		for(unsigned int iver=0;iver<aVertex.size();iver++){
			const unsigned int id_v_cad = aVertex[iver].id_v_cad;
			int ilayer = ilayer_min;
			for(std::auto_ptr<Cad::ICad2D::CItrVertex> itrv = cad_2d.GetItrVertex(id_v_cad);!itrv->IsEnd();(*itrv)++){
				unsigned int id_l0 = itrv->GetIdLoop();
				if( !cad_2d.IsElemID(Cad::LOOP,id_l0) ) continue;
				const int ilayer0 = cad_2d.GetLayer_Loop(id_l0);
				ilayer =  ( ilayer0 > ilayer )	? ilayer0 : ilayer;
			}
			const double height = (ilayer-ilayer_min+0.1)*layer_height;
			CDrawPart_CadVertex dpv;
			dpv.id_cad = id_v_cad;
			dpv.id_msh = aVertex[iver].id;
			dpv.id_v = aVertex[iver].v;
			dpv.is_selected = false;
			dpv.is_show = true;
			dpv.height = height;
			this->m_aIndexVertex.push_back( dpv );
		}
	}

	{	// 座標をセット
		const std::vector<CVector2D>& aVec2D = mesh.GetVectorAry();
		const unsigned int npoin = aVec2D.size();
		const unsigned int ndim = 2;
		m_vertex_ary.SetSize(npoin,ndim);
		for(unsigned int ipoin=0;ipoin<npoin;ipoin++){
			m_vertex_ary.pVertexArray[ipoin*ndim  ] = aVec2D[ipoin].x;
			m_vertex_ary.pVertexArray[ipoin*ndim+1] = aVec2D[ipoin].y;
		}
	}

	return true;
}

void CDrawer_Cad2D::UpdateCAD_Geometry(const Cad::ICad2D& cad_2d)
{
	Msh::CMesher2D mesh;
	mesh.Tesselation(cad_2d);
	
	for(unsigned int idp=0;idp<m_apIndexAry.size();idp++){
		m_apIndexAry[idp]->Clear();
		const unsigned int id_cad = m_apIndexAry[idp]->id_cad;
		Cad::CAD_ELEM_TYPE itype_cad = m_apIndexAry[idp]->itype;
		if( !cad_2d.IsElemID(itype_cad,id_cad) ){ continue; }
		const unsigned int id_msh = mesh.GetElemID_FromCadID(id_cad,itype_cad);
		if( id_msh == 0 ) continue;
		Msh::MSH_TYPE msh_type;
		unsigned int nelem, iloc, id_cad0;
		mesh.GetMshInfo(id_msh,  nelem,msh_type,iloc,id_cad0);
		assert( id_cad0 == id_cad );
		if( msh_type == Msh::TRI ){
			m_apIndexAry[idp]->Set( mesh.GetTriArySet()[iloc] );
            cad_2d.GetColor_Loop(id_cad0,m_apIndexAry[idp]->color);
		}
		else if( msh_type == Msh::BAR ){
			m_apIndexAry[idp]->Set( mesh.GetBarArySet()[iloc] );
		}
	}
	
	{	// 座標をセット
		const std::vector<CVector2D>& aVec2D = mesh.GetVectorAry();
		const unsigned int npoin = aVec2D.size();
		const unsigned int ndim = 2;
		m_vertex_ary.SetSize(npoin,ndim);
		for(unsigned int ipoin=0;ipoin<npoin;ipoin++){
			m_vertex_ary.pVertexArray[ipoin*ndim  ] = aVec2D[ipoin].x;
			m_vertex_ary.pVertexArray[ipoin*ndim+1] = aVec2D[ipoin].y;
		}
	}
}

void CDrawer_Cad2D::GetCadPartID(const int selec_flag[],
                                 Cad::CAD_ELEM_TYPE& part_type, unsigned int& part_id)
{
	for(unsigned int iea=0;iea<m_apIndexAry.size();iea++){
        if( (int)m_apIndexAry[iea]->id_msh == selec_flag[1] ){
			part_type = m_apIndexAry[iea]->itype;
			part_id = m_apIndexAry[iea]->id_cad;		assert( part_id != 0 );
			return;
		}
	}
	for(unsigned int iv=0;iv<this->m_aIndexVertex.size();iv++){
        if( (int)m_aIndexVertex[iv].id_msh == selec_flag[1] ){
			part_type = Cad::VERTEX;
			part_id = m_aIndexVertex[iv].id_cad;	assert( part_id != 0 );
			return;
		}
	}
	part_type = Cad::NOT_SET;
	part_id = 0;
}

void CDrawer_Cad2D::AddSelected(Cad::CAD_ELEM_TYPE itype, unsigned int id)
{
	bool iflag = false;
	if( itype == Cad::VERTEX ){
		for(unsigned int iv=0;iv<this->m_aIndexVertex.size();iv++){
			if( m_aIndexVertex[iv].id_cad == id ){
				m_aIndexVertex[iv].is_selected = true;
				assert( iflag == false );
				iflag = true;
			}
		}
	}
	else{
		for(unsigned int iea=0;iea<m_apIndexAry.size();iea++){
			if( m_apIndexAry[iea]->itype == itype && m_apIndexAry[iea]->id_cad == id ){
				m_apIndexAry[iea]->is_selected = true;
				assert( iflag == false );
				iflag = true;
			}
		}
	}
}

void CDrawer_Cad2D::AddSelected(const int selec_flag[])
{
	for(unsigned int iea=0;iea<m_apIndexAry.size();iea++){
        if( (int)m_apIndexAry[iea]->id_msh == selec_flag[1] ){
			m_apIndexAry[iea]->is_selected = true;
		}
	}
	for(unsigned int iv=0;iv<this->m_aIndexVertex.size();iv++){
        if( (int)m_aIndexVertex[iv].id_msh == selec_flag[1] ){
			m_aIndexVertex[iv].is_selected = true;
		}
	}
}

void CDrawer_Cad2D::ClearSelected()
{
	for(unsigned int iea=0;iea<m_apIndexAry.size();iea++){
		m_apIndexAry[iea]->is_selected = false;
	}
	for(unsigned int iv=0;iv<this->m_aIndexVertex.size();iv++){
		m_aIndexVertex[iv].is_selected = false;
	}
}

void CDrawer_Cad2D::Hide(Cad::CAD_ELEM_TYPE part_type, unsigned int part_id)
{
    if( part_type == Cad::EDGE || part_type == Cad::LOOP ){
		for(unsigned int iea=0;iea<m_apIndexAry.size();iea++){
            if(    m_apIndexAry[iea]->id_cad == part_id
                && m_apIndexAry[iea]->itype  == part_type){
				m_apIndexAry[iea]->is_show = false;
			}
		}
	}
    else if( part_type == Cad::VERTEX ){
		for(unsigned int iv=0;iv<this->m_aIndexVertex.size();iv++){
            if( m_aIndexVertex[iv].id_cad == part_id ){
				m_aIndexVertex[iv].is_show = false;
			}
		}
	}
}

void CDrawer_Cad2D::Show(Cad::CAD_ELEM_TYPE itype_part_cad, unsigned int id_part_cad)
{
    if( itype_part_cad == Cad::EDGE || itype_part_cad == Cad::LOOP ){
		for(unsigned int iea=0;iea<m_apIndexAry.size();iea++){
            if(    m_apIndexAry[iea]->id_cad == id_part_cad
                && m_apIndexAry[iea]->itype  == itype_part_cad ){
				m_apIndexAry[iea]->is_show = true;
			}
		}
	}
    else if( itype_part_cad == Cad::VERTEX ){
		for(unsigned int iv=0;iv<this->m_aIndexVertex.size();iv++){
			if( m_aIndexVertex[iv].id_cad == id_part_cad ){
				m_aIndexVertex[iv].is_show = true;
			}
		}
	}
}

void CDrawer_Cad2D::HideEffected(const Cad::ICad2D& cad_2d,
                                 Cad::CAD_ELEM_TYPE part_type, unsigned int part_id)
{
    if(      part_type == Cad::VERTEX ){
		const unsigned int id_v = part_id;
		for(std::auto_ptr<ICad2D::CItrVertex> itrv=cad_2d.GetItrVertex(id_v);!itrv->IsEnd();(*itrv)++){
			unsigned int id_e0; bool is_same_dir0;
			itrv->GetIdEdge_Behind(id_e0,is_same_dir0);
			this->Hide(Cad::EDGE,id_e0);
			const unsigned int id_l = itrv->GetIdLoop();
			this->Hide(Cad::LOOP,id_l);
		}
	}
    else if( part_type == Cad::EDGE ){
		if( !cad_2d.IsElemID(Cad::EDGE,part_id) ) return;
		unsigned int id_vs, id_ve;
		cad_2d.GetIdVertex_Edge(id_vs,id_ve,part_id);		
		for(std::auto_ptr<ICad2D::CItrVertex> itrv=cad_2d.GetItrVertex(id_vs);!itrv->IsEnd();(*itrv)++){
			unsigned int id_e0; bool is_same_dir0;
			itrv->GetIdEdge_Behind(id_e0,is_same_dir0);
			this->Hide(Cad::EDGE,id_e0);
			const unsigned int id_l = itrv->GetIdLoop();
			this->Hide(Cad::LOOP,id_l);
		}		
		for(std::auto_ptr<ICad2D::CItrVertex> itrv=cad_2d.GetItrVertex(id_ve);!itrv->IsEnd();(*itrv)++){
			unsigned int id_e0; bool is_same_dir0;
			itrv->GetIdEdge_Behind(id_e0,is_same_dir0);
			this->Hide(Cad::EDGE,id_e0);
			const unsigned int id_l = itrv->GetIdLoop();
			this->Hide(Cad::LOOP,id_l);
		}
	}
}

void CDrawer_Cad2D::ShowEffected(const Cad::ICad2D& cad_2d,
                                 Cad::CAD_ELEM_TYPE part_type, unsigned int part_id)
{
	std::vector<unsigned int> aEdgeID, aLoopID;
	if( part_type == 1 ){
		const unsigned int id_v = part_id;
		for(std::auto_ptr<ICad2D::CItrVertex> itrv=cad_2d.GetItrVertex(id_v);!itrv->IsEnd();(*itrv)++){
			unsigned int id_e0; bool is_same_dir0;
			itrv->GetIdEdge_Behind(id_e0,is_same_dir0);
			this->Show(Cad::EDGE,id_e0);
			const unsigned int id_l = itrv->GetIdLoop();
			this->Show(Cad::LOOP,id_l);
		}
	}
}

void CDrawer_Cad2D::Draw() const
{
	// ライティングの指定
	::glEnable(GL_DEPTH_TEST);
	::glDisable(GL_LIGHTING);
    ::glDisable(GL_TEXTURE_2D);
	::glDisable(GL_CULL_FACE);	// とりあえずややこしいからCullFaceはやめとく

	const unsigned int ndim = this->m_vertex_ary.NDim();

	////////////////////////////////////////////////////////////////
	// モデルの描画
    {
        ////////////////
        ::glPointSize(m_pointsize);
        ::glBegin(GL_POINTS);
        for(unsigned int iver=0;iver<this->m_aIndexVertex.size();iver++){
            if( !this->m_aIndexVertex[iver].is_show ) continue;
			const double height = this->m_aIndexVertex[iver].height;
            if( this->m_aIndexVertex[iver].is_selected ){ ::glColor3d(1.0,1.0,0.0); }
            else{ ::glColor3d(0.0,0.0,0.0);	}
            unsigned int ipo0 = this->m_aIndexVertex[iver].id_v;
            ::glVertex3d( m_vertex_ary.pVertexArray[ipo0*ndim+0], 
				m_vertex_ary.pVertexArray[ipo0*ndim+1], 
				height );
        }
        ::glEnd();
    }
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(ndim,GL_DOUBLE,0,m_vertex_ary.pVertexArray);
	for(unsigned int idp=0;idp<m_apIndexAry.size();idp++){
        const CDrawPart* part = m_apIndexAry[idp];
		const double height = part->height;
        if(      part->itype == Cad::EDGE )	// 辺の描画
        {
            if( !part->is_show ) continue;
            ::glLineWidth(m_linewidth);
            if( this->m_is_anti_aliasing ){ // アンチエリアシングの導入
                ::glEnable(GL_LINE_SMOOTH);
                ::glEnable(GL_BLEND);
                ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                ::glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
            }
			if( part->is_selected ){ ::glColor3d(1.0,1.0,0.0); }
			else{ ::glColor3d(0.0,0.0,0.0); }
			::glTranslated(0.0,0.0, height);
			part->DrawElements();
			::glTranslated(0.0,0.0,-height);
            ::glDisable(GL_LINE_SMOOTH);
            ::glDisable(GL_BLEND);
		}
        else if( part->itype == Cad::LOOP ) // ループの描画
        {
            ::glDisable(GL_BLEND);
            if( part->is_selected ){
				::glEnable(GL_POLYGON_STIPPLE);
				::glPolygonStipple((const GLubyte*)m_mask);
                ::glColor3d(1.0,1.0,0.0);
                ::glTranslated(0.0,0.0,+height+0.001);
				part->DrawElements();
                ::glTranslated(0.0,0.0,-height-0.001);
				::glDisable(GL_POLYGON_STIPPLE);
			}
            if( !part->is_show ) continue;
            ::glColor3dv(part->color);
			::glTranslated(0.0,0.0,+height);
			part->DrawElements();
			::glTranslated(0.0,0.0,-height);
		}
	}
    ::glDisableClientState(GL_VERTEX_ARRAY);

	return;
}

void CDrawer_Cad2D::DrawSelection(unsigned int idraw) const
{
    ////////////////
    ::glDisable(GL_BLEND);
    ::glDisable(GL_LINE_SMOOTH);
    ::glDisable(GL_TEXTURE_2D);
	const unsigned int ndim = this->m_vertex_ary.NDim();
	::glPushName(idraw);
	// モデルの描画
	::glEnableClientState(GL_VERTEX_ARRAY);
	::glVertexPointer(ndim,GL_DOUBLE,0,m_vertex_ary.pVertexArray);
	for(unsigned int idp=0;idp<m_apIndexAry.size();idp++){
		const CDrawPart* part = m_apIndexAry[idp];
		const double height = part->height;
		::glPushName( part->id_msh);
		if( part->itype == Cad::EDGE ){
			::glTranslated(0.0,0.0,+height);
			part->DrawElements();
			::glTranslated(0.0,0.0,-height);
		}
		else if( part->itype == Cad::LOOP ){
			::glTranslated(0.0,0.0,+height);
			part->DrawElements();
			::glTranslated(0.0,0.0,-height);
		}
		::glPopName();
	}
	::glDisableClientState(GL_VERTEX_ARRAY);

	::glPointSize(5);
	for(unsigned int iver=0;iver<this->m_aIndexVertex.size();iver++){
		unsigned int ipo0 = this->m_aIndexVertex[iver].id_v;
		double height = this->m_aIndexVertex[iver].height;
		unsigned int id_msh = this->m_aIndexVertex[iver].id_msh;
		::glPushName(id_msh);
		::glBegin(GL_POINTS);
		::glVertex3d( m_vertex_ary.pVertexArray[ipo0*ndim+0], 
			m_vertex_ary.pVertexArray[ipo0*ndim+1], 
			height );
		::glEnd();
		::glPopName();
	}
	
	::glPopName();

	return;
}
