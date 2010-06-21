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


/*! @file
@brief 抽象描画クラス(Com::View::CDrawer)のインターフェース
@remark このファイルの中のオブジェクトは頑張ってOpenGL非依存にしてある
@author Nobuyuki Umetani
*/



#if !defined(DRAWER_H)
#define DRAWER_H

#if defined(__VISUALC__)
	#pragma warning( disable : 4786 )
#endif

#include <vector>
#include <assert.h>

namespace Com{
class CBoundingBox;

namespace View{

//! 描画クラスの抽象クラス
class CDrawer
{
public:
	CDrawer(){ is_show = true; sutable_rot_mode=1; }
	virtual ~CDrawer(){}
	
	/*! 
	@brief セレクションバッファへの書き出し
	@param[in] idraw 名前付けの最初に付けられる数
	*/
	virtual void DrawSelection(unsigned int idraw) const = 0;
	//! 描画する
	virtual void Draw() const = 0;
	/*!
	@brief バウンディング・ボックスの取得
	@param[in] rot ３×３の回転行列
	*/
	virtual Com::CBoundingBox GetBoundingBox(double rot[]) const = 0;
	//! 選択されたオブジェクトを追加して，ハイライトさせる
	virtual void AddSelected(const int selec_flag[]) = 0;
	//! 選択を解除する
	virtual void ClearSelected() = 0;

    virtual void SetAntiAliasing(bool is_aa){
        this->m_is_anti_aliasing = is_aa;
    }

	// return suitable rotation mode ( 0:2D, 1:2DH, 2:3D )
	virtual unsigned int GetSutableRotMode(){ return sutable_rot_mode; }
protected:
	bool is_show;
	unsigned int sutable_rot_mode;	// 1:2D  2:2DH  3:3D
    bool m_is_anti_aliasing;  // アンチエリアシングするかどうか．デフォでfalse
};

class CCamera;

//! CDrawerの派生クラスのポインタを配列として格納するためのクラス
class CDrawerArray
{
public:
	void PushBack(CDrawer* pDrawer){
		assert( pDrawer != 0 );
		this->m_drawer_ary.push_back(pDrawer);
	}
	void Draw() const{
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){
			m_drawer_ary[idraw]->Draw();
		}
	}
	void DrawSelection() const{
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){
			m_drawer_ary[idraw]->DrawSelection(idraw);
		}	
	}
	void AddSelected(const int selec_flg[]){
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){
			m_drawer_ary[idraw]->AddSelected(selec_flg);
		}	
	}
	void ClearSelected(){
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){
			m_drawer_ary[idraw]->ClearSelected();
		}	
	}
	void Clear(){
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){ delete m_drawer_ary[idraw]; }
		m_drawer_ary.clear();
	}
	Com::CBoundingBox GetBoundingBox( double rot[] ) const;
	void InitTrans(Com::View::CCamera& mvp_trans);
public:
	std::vector<CDrawer*> m_drawer_ary;
};


/*! 
@brief OpenGLに頂点配列として渡す配列
@remark このクラスはOpenGL非依存なので，ユーザーが生データ(public変数orz)で取ってきてOpenGLに投げる仕様（あんまり良くない）
*/
class CVertexArray
{
public:
	CVertexArray(const unsigned int np, const unsigned int nd)
		: npoin(np), ndim(nd)
	{
		pVertexArray = new double [npoin*ndim];
	}
	CVertexArray() : pVertexArray(0), npoin(0), ndim(0){}
	virtual ~CVertexArray(){
		if( pVertexArray != 0 ){ delete[] pVertexArray; }
	}
	void SetSize(unsigned int npoin, unsigned int ndim){
		if( this->npoin == npoin && ndim == ndim ) return;
		if( pVertexArray != 0 ) delete[] pVertexArray; 
		this->npoin = npoin;
		this->ndim = ndim;
		pVertexArray = new double [npoin*ndim];
	}
	Com::CBoundingBox GetBoundingBox( double rot[] ) const;
	inline unsigned int NDim() const { return ndim; }
	inline unsigned int NPoin() const { return npoin; }
public:
	double* pVertexArray;
private:
	unsigned int npoin;
	unsigned int ndim;
};
	
}	// end namespace View
}	// end namespace Com


#endif
