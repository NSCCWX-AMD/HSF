/*
The MIT License

Copyright (c) 2019 Hanfeng GU <hanfenggu@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/*
* @File: Field.cpp
* @Author: Hanfeng GU
* @Email:  hanfenggu@gmail.com
* @Date:   2019-09-18 16:04:01
* @Last Modified by:   Hanfeng GU
* @Last Modified time: 2019-11-08 14:08:39
*/

/*
* @brief:
*/

#include "Field.hpp"

using namespace HSF;

template<typename T>
void Field<T>::initSend()
{
	//- create
	if(!patchTabPtr_)
	{
		patchTabPtr_ = new Table<Word, Patch*>;
	}

	//- create
	if(!sendBufferPtr_)
	{
		sendBufferPtr_ = new Table<Word, T*>;
	}

	//-create
	if(!recvBufferPtr_)
	{
		recvBufferPtr_ = new Table<Word, T*>;
	}


	Table<Word, Patch*>& patches = *patchTabPtr_;
	Table<Word, T*>& sendBuffer = *sendBufferPtr_;
	Table<Word, T*>& recvBuffer = *recvBufferPtr_;
	label nPatches = patches.size();

	//- create memory for MPI_Request
	if(!sendRecvRequests_)
	{
		sendRecvRequests_ = new MPI_Request[2*nPatches];
	}


	for(label i=0; i<nPatches; ++i)
	{
		Patch& patchI = patches[i].second;
		label patchSize = patchI.getSize();
		Word patchName = patches[i].first;
		//- here memory created
		if(!sendBuffer[patchName])
		{
			sendBuffer[patchName] = new T[patchSize];
		}
		if(!recvBuffer[patchName])
		{
			recvBuffer[patchName] = new T[patchSize];
		}

		T* patchI_sendBuffer = sendBuffer[patchName];
		T* patchI_recvBuffer = recvBuffer[patchName];

		label* patchI_addressing = patchI.getAddressing();

		for(label j=0; j<patchSize; ++j)
		{
			patchI_sendBuffer[j] = localData_[patchI_addressing[j]];
		}

		MPI_Isend
		(
			patchI_sendBuffer,
			patchSize,
			MPI_SCALAR,
			patchI.getNbrID(),
			1,
			MPI_COMM_WORLD,
			&sendRecvRequests_[i]
		);

		MPI_Irecv
		(
			patchI_recvBuffer,
			patchSize,
			MPI_SCALAR,
			patchI.getNbrID(),
			1,
			MPI_COMM_WORLD,
			&sendRecvRequests_[i + nPatches]
		);
	}
}

template<typename T>
label Field<T>::checkSendStatus()
{
	Table<Word, Patch*>& patches = *patchTabPtr_;
	label nPatches = patches.size();
	MPI_Waitall
	(
		2*nPatches,
		&sendRecvRequests_[0],
		MPI_STATUSES_IGNORE
	);
	DELETE_POINTER(sendRecvRequests_);
	return 1;
}


template<typename T>
void Field<T>::freeSendRecvBuffer()
{
	//- free sendBufferPtr_
	if(sendBufferPtr_)
	{
		label size = (*sendBufferPtr_).size();
		for(int i=0; i<size; ++i)
		{
			DELETE_POINTER((*sendBufferPtr_)[i]);
		}
		DELETE_OBJECT_POINTER(sendBufferPtr_);
	}

	//- free recvBufferPtr_
	if(recvBufferPtr_)
	{
		label size = (*recvBufferPtr_).size();
		for(int i=0; i<size; ++i)
		{
			DELETE_POINTER((*recvBufferPtr_)[i]);
		}
		DELETE_OBJECT_POINTER(recvBufferPtr_);
	}

}


template<typename T>
Field<T>::~Field()
{
	DELETE_POINTER(localData_);
	freeSendRecvBuffer();
}