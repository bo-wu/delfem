/*
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
@brief 連立一次方程式の反復法ソルバの関数
@author Nobuyuki Umetani
*/

#if !defined(SOLVER_LS_ITER_H)
#define SOLVER_LS_ITER_H

#include "delfem/ls/linearsystem_interface_solver.h"

namespace LsSol{
/*! 
@addtogroup LsSol
*/
//@{
//! CG法
bool Solve_CG(double& conv_ratio, unsigned int& num_iter, 
		ILinearSystem_Sol& ls);
//! 前処理付きCG法			  
bool Solve_PCG(double& conv_ratio,unsigned int& iteration, 
		ILinearSystemPreconditioner_Sol& lsp);
//! BiCGSTAB法
bool Solve_BiCGSTAB(double& conv_ratio, unsigned int& num_iter, 
        ILinearSystem_Sol& ls);
//! 前処理付きBiCGSTAB法                    
bool Solve_PBiCGSTAB(double& conv_ratio, unsigned int& num_iter, 
		ILinearSystemPreconditioner_Sol& ls);
//@}
}

#endif
