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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Divert
{
	namespace Net
	{

		/// <summary>
		/// The DivertHandle class is meant to hold an open HANDLE* pointer, which should be
		/// generated from functions such as WinDivertOpen, and is needed as a parameter to some
		/// functions, such as the Send/Recv/Ex functions.
		/// 
		/// The pointer is hidden away from the .NET user and is handled in a way to "guarantee"
		/// safety, so the code is valid for use without /unsafe.
		/// </summary>
		public ref class DivertHandle
		{

		public:

			/// <summary>
			/// Default constructor for building a handle initialized to an invalid state. 
			/// </summary>
			DivertHandle();

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~DivertHandle();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!DivertHandle();

			/// <summary>
			/// Close the handle. If the handle is invalid, this function will always return false.
			/// However, if the handle is valid, the return value from the unmanaged
			/// CloseHandle(HANDLE*) method is returned. If CloseHandle(HANDLE*) succeeds, the
			/// return value is non-zero. If it fails, the return value is zero. As such, this
			/// method will return CloseHandle(HANDLE*) != 0.
			/// </summary>
			/// <returns>
			/// True if the operation suceeded, false otherwise. 
			/// </returns>
			bool Close();

			/// <summary>
			/// Read-only bool to check if the handle is valid.
			/// </summary>
			property bool Valid
			{
				bool get();
			}

		internal:

			/// <summary>
			/// Allow internal construction with the supplied unmanaged handle.
			/// </summary>
			/// <param name="handle">
			/// Unmanaged HANDLE to construct this wrapper around.
			/// </param>
			DivertHandle(HANDLE handle);

			/// <summary>
			/// Internal accessor to the unmanaged handle object held by this object.
			/// </summary>
			/// <returns>
			/// The unmanaged HANDLE member, regardless of its validity.
			/// </returns>
			const HANDLE GetHandle();

		private:

			/// <summary>
			/// Privately held HANDLE member. Exposed internally only so that other members of the
			/// library can access it, but it's kept away from the user.
			/// </summary>
			HANDLE m_handle = INVALID_HANDLE_VALUE;
		};

	} /* namespace Net */
} /* namespace Divert  */
