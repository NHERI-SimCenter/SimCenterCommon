# RandomVariables.cmake .. to mimic exaactly how the .pri file worked .. basically provide a function that adds to a target
# for example for use in quoFEM:
#     include(${CMAKE_CURRENT_LIST_DIR}/../SimCenterCommon/RandomVariables/RandomVariables.cmake)
#     simcenter_add_randomvariables(quoFEM)

set(COMMON_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(simcenter_add_randomvariables target)
  set(dir "${COMMON_MODULE_DIR}")

  target_include_directories(${target} PRIVATE
    "${dir}"
    "${dir}/include"
    "${dir}/../Common"
  )

  # --- SOURCES ---
 set(RV_SOURCES 
    "${dir}/RandomVariableDistribution.cpp"
    "${dir}/NormalDistribution.cpp"
    "${dir}/RandomVariable.cpp"
    "${dir}/LognormalDistribution.cpp"
    "${dir}/GammaDistribution.cpp"
    "${dir}/BetaDistribution.cpp"
    "${dir}/RandomVariablesContainer.cpp"
    "${dir}/UniformDistribution.cpp"
    "${dir}/ConstantDistribution.cpp"
    "${dir}/ContinuousDesignDistribution.cpp"
    "${dir}/WeibullDistribution.cpp"
    "${dir}/GumbelDistribution.cpp"
    "${dir}/UserDef.cpp"
    "${dir}/UserDefVec.cpp"
    "${dir}/LineEditRV.cpp"
    "${dir}/ExponentialDistribution.cpp"
    "${dir}/DiscreteDistribution.cpp"
    "${dir}/ChiSquaredDistribution.cpp"
    "${dir}/TruncatedExponentialDistribution.cpp"

  )

  set(RV_HEADERS
    "${dir}/RandomVariableDistribution.h"
    "${dir}/NormalDistribution.h"
    "${dir}/RandomVariable.h"
    "${dir}/LognormalDistribution.h"
    "${dir}/GammaDistribution.h"
    "${dir}/BetaDistribution.h"
    "${dir}/RandomVariablesContainer.h"
    "${dir}/UniformDistribution.h"
    "${dir}/ConstantDistribution.h"
    "${dir}/ContinuousDesignDistribution.h"
    "${dir}/WeibullDistribution.h"
    "${dir}/GumbelDistribution.h"
    "${dir}/UserDef.h"
    "${dir}/UserDefVec.h"
    "${dir}/LineEditRV.h"
    "${dir}/ExponentialDistribution.h"
    "${dir}/DiscreteDistribution.h"
    "${dir}/ChiSquaredDistribution.h"
    "${dir}/TruncatedExponentialDistribution.h"
  )

   target_sources(${target} PRIVATE ${RV_SOURCES} ${RV_HEADERS})
   
endfunction()
