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
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"


#include "itkShapeRelabelLabelMapFilter.h"
#include "itkLabelImageToShapeLabelMapFilter.h"

#include "itkTestingMacros.h"

int
itkShapeRelabelLabelMapFilterTest1(int argc, char * argv[])
{

  if (argc != 5)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv);
    std::cerr << " input output";
    std::cerr << "background reverseOrdering attribute" << std::endl;
    return EXIT_FAILURE;
  }

  constexpr unsigned int dim = 2;

  using PixelType = unsigned char;

  using ImageType = itk::Image<PixelType, dim>;

  using ShapeLabelObjectType = itk::ShapeLabelObject<PixelType, dim>;
  using LabelMapType = itk::LabelMap<ShapeLabelObjectType>;

  // Reading Image File
  using ReaderType = itk::ImageFileReader<ImageType>;
  auto reader = ReaderType::New();
  reader->SetFileName(argv[1]);

  // Converting LabelImage to ShapeLabelMap
  using I2LType = itk::LabelImageToShapeLabelMapFilter<ImageType, LabelMapType>;
  auto i2l = I2LType::New();
  i2l->SetInput(reader->GetOutput());

  using RelabelType = itk::ShapeRelabelLabelMapFilter<LabelMapType>;
  auto relabel = RelabelType::New();

  // testing get and set macros for ReverseOrdering
  const bool reverseOrdering = std::stoi(argv[3]);
  relabel->SetReverseOrdering(reverseOrdering);
  ITK_TEST_SET_GET_VALUE(reverseOrdering, relabel->GetReverseOrdering());

  // testing boolean macro for ReverseOrdering
  relabel->ReverseOrderingOff();
  ITK_TEST_SET_GET_VALUE(false, relabel->GetReverseOrdering());

  relabel->ReverseOrderingOn();
  ITK_TEST_SET_GET_VALUE(true, relabel->GetReverseOrdering());

  // testing get and set macros for Attribute
  const unsigned int attribute = std::stoi(argv[4]);
  relabel->SetAttribute(attribute);
  ITK_TEST_SET_GET_VALUE(attribute, relabel->GetAttribute());

  const std::string attributeName = ShapeLabelObjectType::GetNameFromAttribute(attribute);
  relabel->SetAttribute(attributeName);

  relabel->SetInput(i2l->GetOutput());

  const itk::SimpleFilterWatcher watcher(relabel, "filter");

  using L2ImageType = itk::LabelMapToLabelImageFilter<LabelMapType, ImageType>;
  auto l2i = L2ImageType::New();
  l2i->SetInput(relabel->GetOutput());

  using WriterType = itk::ImageFileWriter<ImageType>;

  auto writer = WriterType::New();
  writer->SetInput(l2i->GetOutput());
  writer->SetFileName(argv[2]);
  writer->UseCompressionOn();

  ITK_TRY_EXPECT_NO_EXCEPTION(writer->Update());

  return EXIT_SUCCESS;
}
