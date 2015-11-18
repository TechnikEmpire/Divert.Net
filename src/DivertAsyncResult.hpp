/*
* The MIT License (MIT)
*
* Copyright (c) 2015 Jesse Nicholson
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#pragma once

#include "DivertHandle.hpp"
#include <cstdint>

namespace Divert
{
	namespace Net
	{
		/// <summary>
		/// This class handles the underlying system objects and calls to provide users with the
		/// ability to await and attempt to fetch the results of an asynchronous I/O operation
		/// started by the Diversion.ReceiveAsync and Diversion.SendAsync methods.
		/// </summary>
		public ref class DivertAsyncResult
		{

		public:

			/// <summary>
			/// Default constructor.
			/// </summary>
			DivertAsyncResult();

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~DivertAsyncResult();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!DivertAsyncResult();

			/// <summary>
			/// Indicates if an error occurred during the asynchronous operations. 
			/// </summary>
			property bool NoError
			{
				bool get();
			internal:
				void set(bool value);
			}

			/// <summary>
			/// If NoError is true, this will be set to the error code indicated during the error in
			/// the asynchronous operation.
			/// </summary>
			property int ErrorCode
			{
				int get();
			internal:
				void set(int value);
			}
			
			/// <summary>
			/// The length of the data sent or received in the asynchronous operation.
			/// </summary>
			property uint32_t Length
			{
				uint32_t get();
			internal:
				void set(uint32_t value);
			}

			bool Get(uint32_t timeoutInMilliseconds);

		internal:

			/// <summary>
			/// GCHandle to the pinned buffer to be used for this asynchronous operation.
			/// </summary>
			property System::Runtime::InteropServices::GCHandle Buffer
			{
				System::Runtime::InteropServices::GCHandle get();
				void set(System::Runtime::InteropServices::GCHandle value);
			}

			/// <summary>
			/// A reference to the WinDivert HANDLE used to initiate the asynchronous I/O operation.
			/// In order to get the overlapped result, retaining this handle is necessary.
			/// </summary>
			property DivertHandle^ WinDivertHandle
			{
				DivertHandle^ get();
				void set(DivertHandle^ value);
			}

			/// <summary>
			/// A reference to the overlapped event HANDLE. To wait on the overlapped I/O, an event
			/// handle must be created to wait against. It is held here as a property.
			/// </summary>
			property DivertHandle^ OverlappedEventHandle
			{
				DivertHandle^ get();
				void set(DivertHandle^ value);
			}

			/// <summary>
			/// Internal accessor to the unmanaged OVERLAPPED* held by this object. 
			/// </summary>
			/// <returns>
			/// The unmanaged OVERLAPPED* member.
			/// </returns>
			property OVERLAPPED* UnmanagedOverlapped
			{
				OVERLAPPED* get();
				void set(OVERLAPPED* value);
			}			

			/// <summary>
			/// Resets the objects unmanaged internals so that it can be reconfigured for a new
			/// asynchronous operation. If the function returns true, then the object has been reset
			/// to a valid state. If false, then an error occurred and the object state is invalid.
			/// </summary>
			/// <returns>
			/// True if the operation succeeded, false otherwise. 
			/// </returns>
			bool Reset();

		private:

			/// <summary>
			/// Privately held OVERLAPPED member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// 
			/// The overlapped event is used 
			/// </summary>
			OVERLAPPED* m_overlapped = nullptr;

			/// <summary>
			/// In order to get the overlapped result, it's necessary to retain a reference to the
			/// WinDivert HANDLE that was used to initiate the asynchronous operation.
			/// </summary>
			DivertHandle^ m_winDivertHandle;

			/// <summary>
			/// When setup is done to await the overlapped result, an event must be created to wait
			/// on. The HANDLE for that event is held here.
			/// </summary>
			DivertHandle^ m_overlappedEventHandle;

			/// <summary>
			/// If any error occurred while waiting for the overlapped result, then this should
			/// be set to a non-zero value;
			/// </summary>
			int m_errorCode = 0;

			/// <summary>
			/// Indicates if any errors occurred during the asynchronous operation.
			/// </summary>
			bool m_noError = false;

			/// <summary>
			/// Hold the length of the data sent or read in the asynchronous operation.
			/// </summary>
			uint32_t m_ioLength;

			/// <summary>
			/// In order to guarantee that the address of the user-supplied buffer will be valid
			/// during the entire period between initiating the asynchronous I/O and its
			/// completions, it's necessary to pin the array.
			/// </summary>
			System::Runtime::InteropServices::GCHandle m_buffer;

			/// <summary>
			/// Some specialized initiation is required, regardless of constructor.
			/// </summary>
			void Init();

		};

	} /* namespace Net */
} /* namespace Divert */
