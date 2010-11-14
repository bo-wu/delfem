# #####################################################################
# Automatically generated by qmake (2.01a) ? 10 26 12:53:32 2009
# #####################################################################
TEMPLATE = lib
DESTDIR = ..
TARGET = dfm_core
INCLUDEPATH += ../../include/
VPATH += ../../src \
    ../../include/delfem/
#VERSION = 1.2.0
QT += opengl
#DEFINES += NDEBUG
#MAKE_CXXFLAGS_RELEASE = -O3
SOURCES += \
#   Com
    com/drawer.cpp \
    com/vector3d.cpp \
    com/uglyfont.cpp \
    com/quaternion.cpp \
    com/drawer_gl_utility.cpp \
#   Cad
    cad/cad_obj2d.cpp \
    cad/cad_elem2d.cpp \
    cad/brep.cpp \
    cad/drawer_cad.cpp \
    cad/brep2d.cpp \
#   Msh
    msh/meshkernel2d.cpp \
    msh/meshkernel3d.cpp \
    msh/mesher2d.cpp \
    msh/mesher3d.cpp \
    msh/mesh3d.cpp \
    msh/drawer_msh.cpp \
    msh/mesh3d_extrude.cpp \
#   FemField
    femfield/node_ary.cpp \
    femfield/field_world.cpp \
    femfield/field.cpp \
    femfield/eval.cpp \
    femfield/elem_ary.cpp \
    femfield/drawer_field.cpp \
    femfield/drawer_field_vector.cpp \
    femfield/drawer_field_edge.cpp \
    femfield/drawer_field_face.cpp \
    femfield/drawer_field_image_based_flow_vis.cpp \
    femfield/drawer_field_streamline.cpp \
#   MatVec
    matvec/matdiafrac_blkcrs.cpp \
    matvec/matdia_blkcrs.cpp \
    matvec/matfrac_blkcrs.cpp \
    matvec/mat_blkcrs.cpp \
    matvec/vector_blk.cpp \
    matvec/solver_mat_iter.cpp \
    matvec/ordering_blk.cpp \
    matvec/matprolong_blkcrs.cpp \
    matvec/matdiainv_blkdia.cpp \
    matvec/solver_mg.cpp \
    matvec/zvector_blk.cpp \
    matvec/zsolver_mat_iter.cpp \
    matvec/zmatdiafrac_blkcrs.cpp \
    matvec/zmatdia_blkcrs.cpp \
    matvec/zmat_blkcrs.cpp \
#   Ls
    ls/preconditioner.cpp \
    ls/linearsystem.cpp \
    ls/solver_ls_iter.cpp \
    ls/eigen_lanczos.cpp \
#   FemLs
    femls/zsolver_ls_iter.cpp \
    femls/zlinearsystem.cpp \
    femls/linearsystem_fieldsave.cpp \
    femls/linearsystem_field.cpp \
#   FemEqn
    femeqn/ker_emat_tri.cpp \
    femeqn/eqn_poisson.cpp \
    femeqn/eqn_diffusion.cpp \
    femeqn/eqn_advection_diffusion.cpp \
    femeqn/eqn_st_venant.cpp \
    femeqn/eqn_linear_solid2d.cpp \
    femeqn/eqn_linear_solid3d.cpp \
    femeqn/eqn_hyper.cpp \
    femeqn/eqn_navier_stokes.cpp \
    femeqn/eqn_stokes.cpp \
    femeqn/eqn_dkt.cpp \
    femeqn/eqn_helmholtz.cpp \
    femeqn/eqnsys.cpp \
    femeqn/eqnsys_scalar.cpp \
    femeqn/eqnsys_shell.cpp \
    femeqn/eqnsys_fluid.cpp \
    femeqn/eqnsys_solid.cpp
HEADERS += \
#   Com
    camera.h \
    drawer_gl_utility.h \
    drawer.h \
    vector2d.h \
    complex.h \
    vector3d.h \
    quaternion.h \
    serialize.h \
    uglyfont.h \
    indexed_array.h \
#   Cad
    cad_obj2d.h \
    cad2d_interface.h \
    cad_com.h \
    drawer_cad.h \
    cad/brep2d.h \
    cad/brep.h \
    cad/objset_cad.h \
    cad/cad_elem2d.h \
#   Msh
    mesher2d.h \
    mesh3d.h \
    mesh_primitive.h \
    mesh_interface.h \
    drawer_msh.h \
    msh/meshkernel2d.h \
    msh/meshkernel3d.h \
#   FemField
    objset.h \
    node_ary.h \
    field_world.h \
    field.h \
    eval.h \
    elem_ary.h \
    drawer_field.h \
    drawer_field_vector.h \
    drawer_field_edge.h \
    drawer_field_face.h \
    drawer_field_image_based_flow_vis.h \
    drawer_field_streamline.h \
#   MatVec
    matvec/ker_mat.h \
    matvec/solver_mat_iter.h \
    matvec/matfrac_blkcrs.h \
    matvec/mat_blkcrs.h \
    matvec/matdia_blkcrs.h \
    matvec/ordering_blk.h \
    matvec/matprecond_blk.h \
    matvec/matdiainv_blkdia.h \
    matvec/matdiafrac_blkcrs.h \
    matvec/diamat_blk.h \
    matvec/solver_mat_iter.h \
    matvec/solver_mg.h \
    matvec/matprolong_blkcrs.h \
    matvec/zvector_blk.h \
    matvec/zsolver_mat_iter.h \
    matvec/zmatprecond_blk.h \
    matvec/zmatdiafrac_blkcrs.h \
    matvec/zmatdia_blkcrs.h \
    matvec/zmat_blkcrs.h \
#   Ls
    ls/linearsystem.h \
    ls/linearsystem_interface_solver.h \
    ls/solver_ls_iter.h \
    ls/preconditioner.h \
    ls/eigen_lanczos.h \
#   FemLs
    femls/zsolver_ls_iter.h \
    femls/zpreconditioner.h \
    femls/zlinearsystem.h \
    femls/linearsystem_fieldsave.h \
    femls/linearsystem_field.h \
#   FemEqn
    linearsystem_interface_eqnsys.h \
    femeqn/ker_emat_hex.h \
    femeqn/ker_emat_tet.h \
    femeqn/ker_emat_bar.h \
    femeqn/ker_emat_quad.h \
    femeqn/ker_emat_tri.h \
    femeqn/eqn_poisson.h \
    femeqn/eqn_diffusion.h \
    femeqn/eqn_advection_diffusion.h \
    femeqn/eqn_linear_solid2d.h \
    femeqn/eqn_linear_solid3d.h \
    femeqn/eqn_st_venant.h \
    femeqn/eqn_hyper.h \
    femeqn/eqn_navier_stokes.h \
    femeqn/eqn_stokes.h \
    femeqn/eqn_dkt.h \
    femeqn/eqn_helmholtz.h \
    eqnsys.h \
    eqnsys_scalar.h \
    eqnsys_solid.h \
    eqnsys_fluid.h \
    eqnsys_shell.h
