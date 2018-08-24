/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2017 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_FORCEFIELD_AFFINERESTSHAPESPRINGFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_AFFINERESTSHAPESPRINGFORCEFIELD_H
#include  <sofa/helper/system/config.h>

#include <sofa/core/behavior/ForceField.h>
#include <sofa/core/objectmodel/Data.h>
#include <sofa/helper/vector.h>
#include <SofaEigen2Solver/EigenSparseMatrix.h>


namespace sofa
{
namespace core
{
namespace behavior
{
template< class T > class MechanicalState;

} // namespace behavior
} // namespace core
} // namespace sofa

namespace sofa
{

namespace component
{

namespace forcefield
{

/**
* @brief This class describes a simple elastic springs ForceField between DOFs positions and rest positions, with a constant bias force.
*
* Springs are applied to given degrees of freedom between their current positions and their rest shape positions.
* A constant bias force is also applied to the degrees of freedom.
* An external MechanicalState reference can also be passed to the ForceField as rest shape position.
*/
template<class DataTypes>
class AffineRestShapeSpringForceField : public core::behavior::ForceField<DataTypes>
{
public:
	SOFA_CLASS(SOFA_TEMPLATE(AffineRestShapeSpringForceField, DataTypes), SOFA_TEMPLATE(core::behavior::ForceField, DataTypes));

	typedef core::behavior::ForceField<DataTypes> Inherit;
	typedef typename DataTypes::VecCoord VecCoord;
	typedef typename DataTypes::VecDeriv VecDeriv;
	typedef typename DataTypes::Coord Coord;
	typedef typename DataTypes::CPos CPos;
	typedef typename DataTypes::Deriv Deriv;
	typedef typename DataTypes::Real Real;
	typedef typename DataTypes::Vec3 Vec3;
	typedef helper::vector< unsigned int > VecIndex;
	typedef helper::vector< Real >	 VecReal;


	typedef core::objectmodel::Data<VecCoord> DataVecCoord;
	typedef core::objectmodel::Data<VecDeriv> DataVecDeriv;

	Data< helper::vector< unsigned int > > points;
	Data< VecReal > stiffness;
	Data< VecReal > angularStiffness;
	Data< helper::vector< unsigned int > > external_points;
	Data< Vec3 > biasForce;
	Data< Vec3 > biasTorque;
	Data< bool > recompute_indices;

	SingleLink<AffineRestShapeSpringForceField<DataTypes>, sofa::core::behavior::MechanicalState< DataTypes >, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> restMState;
	linearsolver::EigenBaseSparseMatrix<typename DataTypes::Real> matS;

protected:
	AffineRestShapeSpringForceField();

public:
	/// BaseObject initialization method.
	void bwdInit() override;
	virtual void parse(core::objectmodel::BaseObjectDescription *arg) override;
	virtual void reinit() override;

	/// Add the forces.
	virtual void addForce(const core::MechanicalParams* mparams, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v) override;

	virtual void addDForce(const core::MechanicalParams* mparams, DataVecDeriv& df, const DataVecDeriv& dx) override;

	virtual SReal getPotentialEnergy(const core::MechanicalParams* mparams, const DataVecCoord& x) const override
	{
		SOFA_UNUSED(mparams);
		SOFA_UNUSED(x);

		msg_error() << "Get potentialEnergy not implemented";
		return 0.0;
	}

	/// Brings ForceField contribution to the global system stiffness matrix.
	virtual void addKToMatrix(const core::MechanicalParams* mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix) override;

	//virtual void addSubKToMatrix(const core::MechanicalParams* mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix, const helper::vector<unsigned> & addSubIndex) override;

	const DataVecCoord* getExtPosition() const;
	const VecIndex& getIndices() const { return m_indices; }
	const VecIndex& getExtIndices() const { return (useRestMState ? m_ext_indices : m_indices); }

	virtual void updateForceMask() override;

protected:

	void recomputeIndices();

	VecIndex m_indices;
	VecReal k;
	VecReal k_a;
	VecIndex m_ext_indices;
	//VecReal biasF;
	//VecReal biasT;

	SReal lastUpdatedStep;

private:

	bool useRestMState; /// An external MechanicalState is used as rest reference.
};

#ifdef SOFA_EXTERN_TEMPLATE
#ifndef SOFA_FLOAT
extern template class AffineRestShapeSpringForceField<sofa::defaulttype::Rigid3dTypes>;
#endif
#endif

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENT_FORCEFIELD_AFFINERESTSHAPESPRINGFORCEFIELD_H