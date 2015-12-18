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
#ifndef itkMINCTransformIO_h
#define itkMINCTransformIO_h

#include "ITKIOTransformMINCExport.h"

#include "itkTransformIOBase.h"

#include <string>
#include <vector>
#include <itk_minc2.h>
#include "itkMatrixOffsetTransformBase.h"

namespace itk
{

/** \class MINCTransformIOTemplate
 *
* \brief Read and write transforms in Minc XFM Format
*
* \author Vladimir S. FONOV
*         Brain Imaging Center, Montreal Neurological Institute, McGill University, Montreal Canada 2012
*
* \ingroup ITKIOTransformMINC
*/
template<typename TParametersValueType>
class MINCTransformIOTemplate: public TransformIOBaseTemplate<TParametersValueType>
{
public:
  typedef MINCTransformIOTemplate                         Self;
  typedef TransformIOBaseTemplate< TParametersValueType > Superclass;
  typedef SmartPointer<Self>                              Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;

  typedef typename Superclass::TransformType          TransformType;
  typedef typename Superclass::TransformPointer       TransformPointer;
  typedef typename Superclass::TransformListType      TransformListType;
  typedef typename Superclass::ConstTransformListType ConstTransformListType;
  typedef typename TransformType::ParametersType      ParametersType;

  typedef MatrixOffsetTransformBase<TParametersValueType, 3, 3> MatrixOffsetTransformBaseType;

  typedef typename MatrixOffsetTransformBaseType::MatrixType    MatrixType;
  typedef typename MatrixOffsetTransformBaseType::OffsetType    OffsetType;

  /** Run-time type information (and related methods). */
  itkTypeMacro( MINCTransformIOTemplate, TransformIOBaseTemplate );
  itkNewMacro( Self );

  /** Determine the file type. Returns true if this ImageIO can read the
  * file specified. */
  virtual bool CanReadFile( const char * fileName ) ITK_OVERRIDE;

  /** Determine the file type. Returns true if this ImageIO can write the
  * file specified. */
  virtual bool CanWriteFile( const char * fileName ) ITK_OVERRIDE;

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read() ITK_OVERRIDE;

  virtual void Write() ITK_OVERRIDE;

protected:
  MINCTransformIOTemplate();
  virtual ~MINCTransformIOTemplate();

  VIO_General_transform m_XFM;
  bool                  m_XFM_initialized;

private:
  void _cleanup();
  void WriteOneTransform(const int transformIndex,
                         const TransformType *transform,
                         std::vector<VIO_General_transform> &_xfm,
                         const char * xfm_file_base,int & serial);

  void ReadOneTransform(VIO_General_transform *xfm);
};

/** This helps to meet backward compatibility */
typedef MINCTransformIOTemplate<double> MINCTransformIO;

} // end namespace itk

// Note: Explicit instantiation is done in itkMINCTransformIO.cxx

#endif // itkMINCTransformIO_h

/** Explicit instantiations */
#ifndef ITK_TEMPLATE_EXPLICIT_MINCTransformIO
// Explicit instantiation is required to ensure correct dynamic_cast
// behavior across shared libraries.
//
// IMPORTANT: Since within the same compilation unit,
//            ITK_TEMPLATE_EXPLICIT_<classname> defined and undefined states
//            need to be considered. This code *MUST* be *OUTSIDE* the header
//            guards.
//
#  if defined( ITKIOTransformMINC_EXPORTS )
//   We are building this library
#    define ITKIOTransformMINC_EXPORT_EXPLICIT
#  else
//   We are using this library
#    define ITKIOTransformMINC_EXPORT_EXPLICIT ITKIOTransformMINC_EXPORT
#  endif
namespace itk
{

#if defined( __GNUC__ ) && !defined( __INTEL_COMPILER )
#  if ( __GNUC__ > 4 ) || (( __GNUC__ >= 4 ) && ( __GNUC_MINOR__ >= 2 ))
#    if ( __GNUC_MINOR__ >= 6 )
#      pragma GCC diagnostic push
#    endif
#    pragma GCC diagnostic ignored "-Wattributes"
#  endif
#endif

extern template class ITKIOTransformMINC_EXPORT_EXPLICIT MINCTransformIOTemplate< double >;
extern template class ITKIOTransformMINC_EXPORT_EXPLICIT MINCTransformIOTemplate< float >;

#if defined( __GNUC__ ) && !defined( __INTEL_COMPILER )
#  if ( __GNUC__ > 4 ) || (( __GNUC__ >= 4 ) && ( __GNUC_MINOR__ >= 2 ))
#    if ( __GNUC_MINOR__ >= 6 )
#      pragma GCC diagnostic pop
#    else
#      pragma GCC diagnostic warning "-Wattributes"
#    endif
#  endif
#endif

} // end namespace itk
#  undef ITKIOTransformMINC_EXPORT_EXPLICIT
#endif
