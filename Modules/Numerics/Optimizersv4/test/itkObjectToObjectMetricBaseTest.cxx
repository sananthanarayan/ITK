/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkImage.h"
#include "itkObjectToObjectMetricBase.h"
#include "itkTestingMacros.h"

/* Test basic operation of ObjectToObjectMetricBase.
 *
 * TODO Finish exercising all methods.
 */

template< typename TFixedObject,  typename TMovingObject >
class ObjectToObjectMetricTestMetric:
  public itk::ObjectToObjectMetricBase
{
public:
  /** Standard class type aliases. */
  using Self = ObjectToObjectMetricTestMetric;
  using Superclass = itk::ObjectToObjectMetricBase;
  using Pointer = itk::SmartPointer< Self >;
  using ConstPointer = itk::SmartPointer< const Self >;

  using MeasureType = typename Superclass::MeasureType;
  using DerivativeType = typename Superclass::DerivativeType;
  using ParametersType = typename Superclass::ParametersType;
  using ParametersValueType = typename Superclass::ParametersValueType;

  itkTypeMacro(ObjectToObjectMetricTestMetric, ObjectToObjectMetricBase);

  itkNewMacro(Self);

  // Pure virtual functions that all Metrics must provide
  unsigned int GetNumberOfParameters() const override { return 5; }

  MeasureType GetValue() const override
  {
    this->m_Value = 1.0;
    return this->m_Value;
  }

  void GetDerivative( DerivativeType & derivative ) const override
  {
    derivative.Fill(0.0);
  }

  void GetValueAndDerivative( MeasureType & value, DerivativeType & derivative ) const override
  {
    value = 1.0; derivative.Fill(0.0);
  }

  unsigned int GetNumberOfLocalParameters() const override
  { return 0; }

  void UpdateTransformParameters( const DerivativeType &, ParametersValueType ) override {}

  const ParametersType & GetParameters() const override
  { return m_Parameters; }

  bool HasLocalSupport() const override
    { return false; }

  void SetParameters( ParametersType & ) override
  {
  }

  void Initialize() throw ( itk::ExceptionObject ) override {}

  void PrintSelf(std::ostream& os, itk::Indent indent) const override
  { Superclass::PrintSelf( os, indent ); }

  ParametersType  m_Parameters;

private:
  ObjectToObjectMetricTestMetric() {}
  ~ObjectToObjectMetricTestMetric() override {}
};

int itkObjectToObjectMetricBaseTest(int ,char * [])
{
  using ImageType = itk::Image< unsigned char, 3 >;
  using ObjectMetricType = ObjectToObjectMetricTestMetric<ImageType, ImageType>;

  ObjectMetricType::Pointer objectMetric = ObjectMetricType::New();

  objectMetric->Print( std::cout );

  std::cout << objectMetric << std::endl;

  std::cout << objectMetric->GetNameOfClass() << std::endl;

  using ParametersType = ObjectMetricType::ParametersType;

  ParametersType parameters(13);
  parameters.Fill( 19.5);

  TEST_EXPECT_EQUAL( objectMetric->GetValue( ), 1.0 );

  TEST_EXPECT_EQUAL( objectMetric->GetCurrentValue( ), 1.0 );

  return EXIT_SUCCESS;
}
