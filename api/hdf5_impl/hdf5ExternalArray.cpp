/*
 * Copyright (C) 2012 by Glenn Hickey (hickey@soe.ucsc.edu)
 *
 * Released under the MIT license, see LICENSE.txt
 */

#include <cassert>
#include <iostream>
#include "hdf5ExternalArray.h"

using namespace hal;
using namespace H5;
using namespace std;

/** Constructor */
HDF5ExternalArray::HDF5ExternalArray() :
  _file(NULL),
  _size(0),
  _chunkSize(0),
  _bufStart(0),
  _bufEnd(0),
  _bufSize(0),
  _buf(NULL),
  _dirty(false)
{}

/** Destructor */
HDF5ExternalArray::~HDF5ExternalArray()
{
  delete [] _buf;
}

// Create a new dataset in specifed location
void HDF5ExternalArray::create(H5::CommonFG* file, 
                               const H5std_string& path, 
                               const H5::DataType& dataType,
                               hsize_t numElements,
                               const H5::DSetCreatPropList& cparms)
{
  // copy in parameters
  _file = file;
  _path = path;
  _dataType = dataType;
  _size = numElements;
  _dataSize = _dataType.getSize();
  _dataSpace = DataSpace(1, &_size);
  
  // resolve chunking size (0 = do not chunk)
  if (cparms.getLayout() == H5D_CHUNKED)
  {
    cparms.getChunk(1, &_chunkSize);
    if (_chunkSize == 1)
    {
      throw DataSetIException("HDF5ExternalArray::create",
                             "chunkSize of 1 not supported");
    }
    if (_chunkSize > _size)
    {
      throw DataSetIException("HDF5ExternalArray::create",
                              "chunkSize > array size is not supported");
    }
  }
  else
  {
    _chunkSize = 0;
  }
  
  // create the internal data buffer
  _bufSize = _chunkSize > 1 ? _chunkSize : _size;  
  _bufStart = 0;
  _bufEnd = _bufStart + _bufSize - 1;
  delete [] _buf;
  _buf = new char[_bufSize * _dataSize];

  // create the hdf5 array
  _dataSet = _file->createDataSet(_path, _dataType, _dataSpace, cparms);
  _chunkSpace = DataSpace(1, &_bufSize);
}

// Load an existing dataset into memory
void HDF5ExternalArray::load(H5::CommonFG* file, const H5std_string& path,
                             hsize_t chunksInBuffer)
{
  // load up the parameters
  _file = file;
  _path = path;
  _dataSet = _file->openDataSet(_path);
  _dataType = _dataSet.getDataType();
  _dataSpace = _dataSet.getSpace();
  _dataSize = _dataType.getSize();
  assert(_dataSpace.getSimpleExtentNdims() == 1);  
  _dataSpace.getSimpleExtentDims(&_size, NULL);
  DSetCreatPropList cparms = _dataSet.getCreatePlist();
  
  // resolve chunking size (0 = do not chunk)
  if (cparms.getLayout() == H5D_CHUNKED)
  {
    cparms.getChunk(1, &_chunkSize);
    _chunkSize *= chunksInBuffer;
    if (_chunkSize == 1)
    {
      throw DataSetIException("HDF5ExternalArray::create",
                             "chunkSize of 1 not supported");
    }
    if (_chunkSize > _size)
    {
      throw DataSetIException("HDF5ExternalArray::create",
                              "chunkSize > array size is not supported");
    }
  }
  else
  {
    _chunkSize = 0;
  }
  
  // create the internal data buffer
  _bufSize = _chunkSize > 1 ? _chunkSize : _size;  
  _bufStart = 0;
  _bufEnd = _bufStart + _bufSize - 1;
  delete [] _buf;
  _buf = new char[_bufSize * _dataSize];

  // fill buffer from disk
  page(0);
}

// Write the memory buffer back to the file 
void HDF5ExternalArray::write()
{
  if (_dirty == true)
  {
    _dataSpace.selectHyperslab(H5S_SELECT_SET, &_bufSize, &_bufStart);
    _dataSet.write(_buf, _dataType, _chunkSpace, _dataSpace);
  }
}

// Page chunk containing index i into memory 
void HDF5ExternalArray::page(hsize_t i)
{
  if (_dirty == true)
  {
    write();
  }
  _bufSize = _chunkSize > 1 ? _chunkSize : _size;  
  _bufStart = (i / _bufSize) * _bufSize; // todo: review
  _bufEnd = _bufStart + _bufSize - 1;  

  if (_bufEnd >= _size)
  {
    _bufEnd = _size - 1;
    _bufSize = _bufEnd - _bufStart + 1;
  }

  _chunkSpace = DataSpace(1, &_bufSize);
  _dataSpace.selectHyperslab(H5S_SELECT_SET, &_bufSize, &_bufStart);
  _dataSet.read(_buf, _dataType, _chunkSpace, _dataSpace);
  _dirty = false;
}