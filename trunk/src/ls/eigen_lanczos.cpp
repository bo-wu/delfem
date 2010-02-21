

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif

#include <iostream>
#include <math.h>

#include "Eigen_Lanczos.h"
#include "LinearSystem.h"

#include "SolverLS_Iter.h"

using namespace Fem::Ls;
using namespace Fem::Sol;




bool EigenValueHess(const unsigned int h_size, const double* in_hess, double* eval_ary){
	double* rot = new double [(h_size-1)*2];
	double* hess = new double [h_size*h_size];
	for(unsigned int i=0;i<h_size*h_size;i++){
		hess[i] = in_hess[i];
	}
	for(int iteration=0;iteration<1000;iteration++){
		{
			double dtmp1 = 0.0;
			for(unsigned int i=0;i<h_size-1;i++){
				dtmp1 +=  hess[i+1+i*h_size]*hess[i+1+i*h_size];
			}
			std::cout << iteration << " " << dtmp1 << std::endl;
		}
			
		for(unsigned int j=0;j<h_size;j++){
			////////////////////////////////
			// H = Q^T * R
			////////////////////////////////
			for(unsigned int i=0;i<j;i++){
				const double dtemp1 =   rot[  i*2]*hess[i  +j*h_size] + rot[1+i*2]*hess[i+1+j*h_size];
				const double dtemp2 = - rot[1+i*2]*hess[i  +j*h_size] + rot[  i*2]*hess[i+1+j*h_size];
				hess[i  +j*h_size] = dtemp1;
				hess[i+1+j*h_size] = dtemp2;
			}
			if( j != h_size-1 ){
				const double dtemp1 = hess[j  +j*h_size];
				const double dtemp2 = hess[j+1+j*h_size];
				const double dtemp3 = sqrt(dtemp1*dtemp1+dtemp2*dtemp2);
				rot[  j*2] = dtemp1 / dtemp3;	// cosine 
				rot[1+j*2] = dtemp2 / dtemp3;	// sine7
				hess[j  +j*h_size] = dtemp3;
				hess[j+1+j*h_size] = 0.0;
			}
		}
		
		for(unsigned int j=0;j<h_size-1;j++){
			////////////////////////////////
			// H = R * Q
			////////////////////////////////
			for(unsigned int i=0;i<j+2;i++){
				const double dtemp1 =   rot[  j*2]*hess[i+j*h_size] + rot[1+j*2]*hess[i+(j+1)*h_size];
				const double dtemp2 = - rot[1+j*2]*hess[i+j*h_size] + rot[  j*2]*hess[i+(j+1)*h_size];
				hess[i+    j*h_size] = dtemp1;
				hess[i+(j+1)*h_size] = dtemp2;
			}
		}
	}
	for(unsigned int i=0;i<h_size;i++){
		eval_ary[i] =  hess[i+i*h_size];
	}
	delete[] rot;
	delete[] hess;

	return true;
}


bool Fem::Sol::ArnoldiQR(Fem::Ls::CLinearSystem_Eigen& ls ){

	const unsigned int m_num = 10;
	
	double* hessenberg = new double [m_num*m_num];

	ls.ReSizeTmpVecSolver(m_num+2);
	const int itmp = m_num+1;

	int iupd = -2;
	int ires = -1;
	
	//////////////////////////////// 
	// Initial Base of Krylov Subspace
	////////////////////////////////
	ls.COPY(-2,0);	// upd->res
	double sq_normx;
	{	// 残差を正規化
		sq_normx = ls.DOT(0,0);
		const double normx = sqrt(sq_normx);
		ls.SCAL(1.0/normx,0);
	}

	unsigned int h_size = m_num;
	for(unsigned int miter=0;miter<m_num;miter++){
		ls.COPY(miter,itmp);
		ls.MATVEC(1.0,itmp,0.0,miter+1);
		for(unsigned int jiter=0;jiter<miter+1;jiter++){
			double dtmp1 = ls.DOT(miter+1,jiter);
			ls.AXPY(-dtmp1,jiter,miter+1);
			hessenberg[jiter+m_num*miter] = dtmp1;
		}
		{
			double dtmp1 = ls.DOT(miter+1,miter+1);
			dtmp1 = sqrt(dtmp1);
			if( dtmp1 < 1.0e-30 ){
				h_size  = miter+1;
				break;
			}
			if( miter == m_num-1 ) break;
			hessenberg[miter+1+m_num*miter] = dtmp1;
			ls.SCAL(1.0/dtmp1,miter+1);
		}
	}

	{
		double* tmp_hessenberg = new double [h_size*h_size];
		for(unsigned int jiter=0;jiter<h_size;jiter++){
			for(unsigned int iiter=0;iiter<=jiter;iiter++){
				tmp_hessenberg[iiter+jiter*h_size] = hessenberg[iiter+jiter*m_num];
			}
			if( jiter != h_size-1 ){
				tmp_hessenberg[jiter+1+jiter*h_size] = hessenberg[jiter+1+jiter*m_num];
			}
		}
		delete[] hessenberg;
		hessenberg = tmp_hessenberg;
	}

	double* eval_ary = new double [h_size];
	EigenValueHess( h_size, hessenberg, eval_ary);

	for(unsigned int miter=0;miter<h_size;miter++){
		std::cout << miter << " " << eval_ary[miter] << std::endl;
	}
	return true;
}











unsigned int NumSignChangeStrum(double lambda, unsigned int n, const double* aAlBe)
{
	unsigned int cnt = 0;
	double p0 = 1.0;
	double p1 = lambda - aAlBe[0];
	if( p0*p1 < 0 ){ cnt++; }
//	std::cout << 0 << " " << p0 << std::endl;
//	std::cout << 1 << " " << p1 << std::endl;
	for(unsigned int i=1;i<n;i++){
		const double p2 = (lambda-aAlBe[2*i])*p1-aAlBe[2*i+1]*aAlBe[2*i+1]*p0;
//		std::cout << i+1 << " " << p2 << std::endl;
		if( p1*p2 < 0 ){ cnt++; }
		p0 = p1;
		p1 = p2;
	}
//	std::cout << p1 << std::endl;
//	std::cout << "%%%%%%% " << n << std::endl;
	return cnt;
}

unsigned int FindLambdaInRange(unsigned int n0, unsigned int n1, 
					   double range_min, double range_max, 
					   std::vector<double>& aLambda, 
					   const unsigned int n, const double* aAlBe )
{
	if( n0 == n1 ) return 0;
	std::cout << "Find " << range_min << " " << range_max << " " << n0-n1 << std::endl;
	double range_middle = (range_min+range_max)*0.5;
	double range_span   = (range_max-range_min)*0.5;
	unsigned int n2 = NumSignChangeStrum( range_middle, n, aAlBe);
	if( n0 != n2 ){
		if( range_span > 0.0001 ){
			FindLambdaInRange(n0,n2,range_min,range_middle,aLambda,n,aAlBe);
		}
		else{
			aLambda.push_back(range_middle);
		}
	}
	if( n2 != n1 ){
		if( range_span > 0.0001 ){
			FindLambdaInRange(n2,n1,range_middle,range_max,aLambda,n,aAlBe);
		}
		else{
			aLambda.push_back(range_middle);
		}
	}
	return 0;
}




bool Fem::Sol::EigenValue_Lanczos( unsigned int nlambda, std::vector<double>& aLambda, unsigned int max_iter, 
		Fem::Ls::CLinearSystem_Eigen& ls)
{
	ls.ReSizeTmpVecSolver(1);

	const int ix = -2;	// update
	const int iAx = -1;	// residual
	const int ix0 = 0;

	{
		const double sq_normx = ls.DOT(ix,ix);
		const double normx = sqrt(sq_normx);
		ls.SCAL(1.0/normx,ix);
	}

	double* aAlBe = new double[2*max_iter];
	aAlBe[1] = 0.0;

	for(unsigned int iiter=0;;iiter++)
	{
		// {iAx} = [A]*{ix}
		ls.MATVEC(1.0,ix,0.0,iAx);
//		if( iiter > 0 ){
			// Ax = Ax - beta(i-1)*x(i-1)
			ls.AXPY(-aAlBe[2*iiter+1],ix0,iAx);
//		}
		// alpha = (x,Ax)
		const double alpha  = ls.DOT(ix,iAx);
		aAlBe[2*iiter] = alpha;

		if( iiter == max_iter-1 ) break;

		// {iAx} = {iAx}-alpha(i)*{ix}(i)
		ls.AXPY(-alpha,ix,iAx);

		// calc residual 
		const double sq_beta = ls.DOT(iAx,iAx);
		// beta(i) = ||{iAx}||
		const double beta = sqrt(sq_beta);
		aAlBe[2*iiter+3] = beta;

		ls.COPY(ix,ix0);
		ls.SCAL(1.0/beta,iAx);
		ls.COPY(iAx,ix);
	}

	aLambda.clear();
	aLambda.reserve(nlambda);

	{
		unsigned int n0 = NumSignChangeStrum( 0, max_iter, aAlBe);
		unsigned int n1 = NumSignChangeStrum( 1, max_iter, aAlBe);
		std::cout << n0 << " " << n1 << std::endl;
	}


	double range_min = -0.01;
	unsigned int n0 = NumSignChangeStrum( range_min, max_iter, aAlBe);
	if( n0 != max_iter ){
		return false;
		std::cout << "Error!-->NotImplimented" << std::endl;
	}
	double range_max = 0.0001;
	for(;;){
		unsigned int n1 = NumSignChangeStrum( range_max, max_iter, aAlBe);
		if( n0 - n1 > 0 ){
			FindLambdaInRange(n0,n1,   range_min,range_max,    aLambda,  max_iter,   aAlBe );
			if( aLambda.size() >= nlambda ) break;
			range_min = range_max;
			range_max = range_max*2;
			n0 = n1;
		}
		else{
			std::cout << "Not in " << range_min << " " << range_max << std::endl;
			range_min = range_max;
			range_max *= 2;
			n0 = n1;
		}
		if( range_max > 100 ) return false;
	}

	{
		unsigned int n0 = NumSignChangeStrum( 0.001, max_iter, aAlBe);
		unsigned int n1 = NumSignChangeStrum( 0.005, max_iter, aAlBe);
		std::cout << n0 << " " << n1 << std::endl;
	}

	delete[] aAlBe;

	return true;
}

double Fem::Sol::MinimumEigenValueVector_InvPower(Fem::Ls::CLinearSystem_Eigen& ls , 
												  Fem::Ls::CPreconditioner& pls, 
												  const std::vector<unsigned int>& aIdVec, 
												  unsigned int& num_iter )
{
	const unsigned int max_iter = num_iter;
	if( ls.GetTmpVectorArySize() < 3 ){ ls.ReSizeTmpVecSolver(3); }

	const int ires = -1;
	const int iupd = -2;
	const int itmp = 2;

	// 無視ベクトルのグラムシュミットの直交化
	for(unsigned int iivec=0;iivec<aIdVec.size();iivec++){
		const unsigned int ivec0=aIdVec[iivec];
		for(unsigned int jivec=0;jivec<iivec;jivec++){
			const unsigned int jvec0=aIdVec[jivec];
			const double dot = ls.DOT(ivec0,jvec0);
			ls.AXPY(-dot,jvec0,ivec0);
		}
		const double sq_normx = ls.DOT(ivec0,ivec0);
		const double normx = sqrt(sq_normx);
		ls.SCAL(1.0/normx,ivec0);
	}

	// 更新から無視ベクトル成分を引く
	for(unsigned int iivec=0;iivec<aIdVec.size();iivec++){
		const unsigned int ivec0=aIdVec[iivec];
		const double dot0 = ls.DOT(ivec0,iupd);
		ls.AXPY(-dot0,ivec0,iupd);
	}
	{	// 更新を正規化
		const double sq_normx = ls.DOT(iupd,iupd);
		const double normx = sqrt(sq_normx);
		ls.SCAL(1.0/normx,iupd);
	}

	double min_eigen0, min_eigen;
	unsigned int iiter;
	for(iiter=0;iiter<max_iter;iiter++){
		ls.COPY(iupd,ires);
		ls.COPY(iupd,itmp);
		{
			double conv_ratio = 1.0e-3;
			unsigned int num_iter = 300;
            Ls::Sol::Solve_PCG(conv_ratio, num_iter, ls, pls);
			std::cout << "PCG iter : " << num_iter << " " << conv_ratio << std::endl;
		}
		// 無視ベクトルの成分を引く
		for(unsigned int iivec=0;iivec<aIdVec.size();iivec++){
			const unsigned int ivec0=aIdVec[iivec];
			const double dot0 = ls.DOT(ivec0,iupd);
			ls.AXPY(-dot0,ivec0,iupd);
		}
		// レリー積の計算
		min_eigen = ls.DOT(itmp,iupd);
		{
			const double norm0 = sqrt(ls.DOT(iupd,iupd));
			// 更新ベクトルを正規化
			ls.SCAL(1.0/norm0,iupd);
		}
	}

	num_iter = iiter+1;	// 反復回数を代入
	return  1.0 / min_eigen;
}

