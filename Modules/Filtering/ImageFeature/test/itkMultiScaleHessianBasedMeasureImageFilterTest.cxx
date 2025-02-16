/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkHessianToObjectnessMeasureImageFilter.h"
#include "itkMultiScaleHessianBasedMeasureImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"
#include "itkTestingMacros.h"

int
itkMultiScaleHessianBasedMeasureImageFilterTest(int argc, char * argv[])
{
  if (argc < 4)
  {
    std::cerr
      << "Missing Parameters: " << itkNameOfTestExecutableMacro(argv) << " InputImage"
      << " EnhancedOutputImage ScalesOutputImage "
      << " [SigmaMin SigmaMax NumberOfScales ObjectDimension Bright/Dark EnhancedOutputImage2 ScalesOutputImage3]"
      << std::endl;
    return EXIT_FAILURE;
  }

  // Define the dimension of the images
  constexpr unsigned int Dimension = 2;

  using InputPixelType = float;
  using InputImageType = itk::Image<InputPixelType, Dimension>;


  using OutputPixelType = float;
  using OutputImageType = itk::Image<OutputPixelType, Dimension>;

  using FileReaderType = itk::ImageFileReader<InputImageType>;

  using FileWriterType = itk::ImageFileWriter<OutputImageType>;

  using RealPixelType = itk::NumericTraits<InputPixelType>::RealType;

  using HessianPixelType = itk::SymmetricSecondRankTensor<RealPixelType, Dimension>;
  using HessianImageType = itk::Image<HessianPixelType, Dimension>;

  // Declare the type of enhancement filter
  using ObjectnessFilterType = itk::HessianToObjectnessMeasureImageFilter<HessianImageType, OutputImageType>;

  // Declare the type of multiscale enhancement filter
  using MultiScaleEnhancementFilterType =
    itk::MultiScaleHessianBasedMeasureImageFilter<InputImageType, HessianImageType, OutputImageType>;

  auto imageReader = FileReaderType::New();
  imageReader->SetFileName(argv[1]);
  try
  {
    imageReader->Update();
  }
  catch (const itk::ExceptionObject & ex)
  {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
  }

  auto objectnessFilter = ObjectnessFilterType::New();
  objectnessFilter->SetScaleObjectnessMeasure(false);
  objectnessFilter->SetBrightObject(true);
  objectnessFilter->SetAlpha(0.5);
  objectnessFilter->SetBeta(0.5);
  objectnessFilter->SetGamma(5.0);


  auto multiScaleEnhancementFilter = MultiScaleEnhancementFilterType::New();
  ITK_EXERCISE_BASIC_OBJECT_METHODS(
    multiScaleEnhancementFilter, MultiScaleHessianBasedMeasureImageFilter, ImageToImageFilter);


  multiScaleEnhancementFilter->SetInput(imageReader->GetOutput());
  multiScaleEnhancementFilter->SetHessianToMeasureFilter(objectnessFilter);
  ITK_TEST_SET_GET_VALUE(objectnessFilter, multiScaleEnhancementFilter->GetHessianToMeasureFilter());

  multiScaleEnhancementFilter->SetSigmaStepMethodToLogarithmic();

  const itk::SimpleFilterWatcher watcher(multiScaleEnhancementFilter);

  constexpr double tolerance = 0.01;

  if (argc > 4)
  {
    const double sigmaMinimum = std::stod(argv[4]);
    multiScaleEnhancementFilter->SetSigmaMinimum(sigmaMinimum);

    if (itk::Math::abs(multiScaleEnhancementFilter->GetSigmaMinimum() - sigmaMinimum) > tolerance)
    {
      std::cerr << " Error in Set/GetSigmaMinimum() " << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (argc > 5)
  {
    const double sigmaMaximum = std::stod(argv[5]);
    multiScaleEnhancementFilter->SetSigmaMaximum(sigmaMaximum);

    if (itk::Math::abs(multiScaleEnhancementFilter->GetSigmaMaximum() - sigmaMaximum) > tolerance)
    {
      std::cerr << " Error in Set/GetSigmaMaximum() " << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (argc > 6)
  {
    const unsigned int numberOfSigmaSteps = std::stoi(argv[6]);
    multiScaleEnhancementFilter->SetNumberOfSigmaSteps(numberOfSigmaSteps);

    if (multiScaleEnhancementFilter->GetNumberOfSigmaSteps() != numberOfSigmaSteps)
    {
      std::cerr << " Error in Set/GetNumberOfSigmaSteps() " << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (argc > 7)
  {
    objectnessFilter->SetObjectDimension(std::stoi(argv[7]));
  }

  if (argc > 8)
  {
    objectnessFilter->SetBrightObject(std::stoi(argv[8]));
  }

  multiScaleEnhancementFilter->GenerateScalesOutputOn();
  if (!multiScaleEnhancementFilter->GetGenerateScalesOutput())
  {
    std::cerr << "Error in Set/GetGenerateScalesOutput()" << std::endl;
    return EXIT_FAILURE;
  }

  multiScaleEnhancementFilter->SetGenerateScalesOutput(false);
  if (multiScaleEnhancementFilter->GetGenerateScalesOutput())
  {
    std::cerr << "Error in Set/GetGenerateScalesOutput()" << std::endl;
    return EXIT_FAILURE;
  }
  multiScaleEnhancementFilter->SetGenerateScalesOutput(true);

  multiScaleEnhancementFilter->GenerateHessianOutputOn();
  if (!multiScaleEnhancementFilter->GetGenerateHessianOutput())
  {
    std::cerr << "Error in Set/GetGenerateHessianOutput()" << std::endl;
    return EXIT_FAILURE;
  }
  multiScaleEnhancementFilter->SetGenerateHessianOutput(false);
  if (multiScaleEnhancementFilter->GetGenerateHessianOutput())
  {
    std::cerr << "Error in Set/GetGenerateHessianOutput()" << std::endl;
    return EXIT_FAILURE;
  }
  multiScaleEnhancementFilter->SetGenerateHessianOutput(true);

  bool nonNegativeHessianBasedMeasure = true;
  ITK_TEST_SET_GET_BOOLEAN(multiScaleEnhancementFilter, NonNegativeHessianBasedMeasure, nonNegativeHessianBasedMeasure);


  try
  {
    multiScaleEnhancementFilter->Update();
  }
  catch (const itk::ExceptionObject & e)
  {
    std::cerr << e << std::endl;
  }

  auto writer = FileWriterType::New();
  writer->SetFileName(argv[2]);
  writer->UseCompressionOn();
  writer->SetInput(multiScaleEnhancementFilter->GetOutput());

  try
  {
    writer->Update();
  }
  catch (const itk::ExceptionObject & e)
  {
    std::cerr << e << std::endl;
  }

  writer->SetFileName(argv[3]);
  writer->UseCompressionOn();
  writer->SetInput(multiScaleEnhancementFilter->GetScalesOutput());

  try
  {
    writer->Update();
  }
  catch (const itk::ExceptionObject & e)
  {
    std::cerr << e << std::endl;
  }

  const HessianImageType * hessianImage = multiScaleEnhancementFilter->GetHessianOutput();

  std::cout << "Hessian Image Buffered Region = " << std::endl;
  std::cout << hessianImage->GetBufferedRegion() << std::endl;

  if (argc > 9)
  {
    // Change sigma step to equispaced type and regenerate vesselness image
    auto sigmaStepMethod = static_cast<MultiScaleEnhancementFilterType::SigmaStepMethodEnum>(
      MultiScaleEnhancementFilterType::SigmaStepMethodEnum::EquispacedSigmaSteps);
    multiScaleEnhancementFilter->SetSigmaStepMethod(sigmaStepMethod);
    ITK_TEST_SET_GET_VALUE(sigmaStepMethod, multiScaleEnhancementFilter->GetSigmaStepMethod());

    try
    {
      multiScaleEnhancementFilter->Update();
    }
    catch (const itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
    }

    auto writer2 = FileWriterType::New();
    writer2->SetFileName(argv[9]);
    writer2->UseCompressionOn();
    writer2->SetInput(multiScaleEnhancementFilter->GetOutput());

    try
    {
      writer2->Update();
    }
    catch (const itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
    }
  }

  if (argc > 10)
  {
    // Change NonNegativeHessianBasedMeasure to Off and regenerate vesselness image
    nonNegativeHessianBasedMeasure = false;
    ITK_TEST_SET_GET_BOOLEAN(
      multiScaleEnhancementFilter, NonNegativeHessianBasedMeasure, nonNegativeHessianBasedMeasure);

    try
    {
      multiScaleEnhancementFilter->Update();
    }
    catch (const itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
    }

    auto writer3 = FileWriterType::New();
    writer3->SetFileName(argv[10]);
    writer3->UseCompressionOn();
    writer3->SetInput(multiScaleEnhancementFilter->GetScalesOutput());

    try
    {
      writer3->Update();
    }
    catch (const itk::ExceptionObject & e)
    {
      std::cerr << e << std::endl;
    }
  }

  // Test for NumberOfSigmaSteps = 0
  multiScaleEnhancementFilter->SetNumberOfSigmaSteps(0);
  try
  {
    multiScaleEnhancementFilter->Update();
  }
  catch (const itk::ExceptionObject & e)
  {
    std::cerr << e << std::endl;
  }

  // Test for NumberOfSigmaSteps = 1
  multiScaleEnhancementFilter->SetNumberOfSigmaSteps(1);
  try
  {
    multiScaleEnhancementFilter->Update();
  }
  catch (const itk::ExceptionObject & e)
  {
    std::cerr << e << std::endl;
  }

  // Test streaming enumeration for MultiScaleHessianBasedMeasureImageFilterEnums::SigmaStepMethod elements
  const std::set<itk::MultiScaleHessianBasedMeasureImageFilterEnums::SigmaStepMethod> allSigmaStepMethod{
    itk::MultiScaleHessianBasedMeasureImageFilterEnums::SigmaStepMethod::EquispacedSigmaSteps,
    itk::MultiScaleHessianBasedMeasureImageFilterEnums::SigmaStepMethod::LogarithmicSigmaSteps
  };
  for (const auto & ee : allSigmaStepMethod)
  {
    std::cout << "STREAMED ENUM VALUE MultiScaleHessianBasedMeasureImageFilterEnums::SigmaStepMethod: " << ee
              << std::endl;
  }

  return EXIT_SUCCESS;
}
