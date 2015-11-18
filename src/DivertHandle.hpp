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

#include <windivert.h>

namespace Divert
{
	namespace Net
	{

		/// <summary>
		/// The DivertHandle class should be generated from functions such as WinDivertOpen. Handles
		/// are needed as a parameter to some functions, such as the Send/Recv/Ex functions.
		/// 
		/// Handles are also generated when using the Ex (async) functions. Regardless of where the
		/// handle originated, if you have a valid handle, you must keep a reference of it. If you
		/// don't, when the object is destroyed, it will close the handle.
		/// </summary>
		public ref class DivertHandle
		{

		public:

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
			/// Default constructor. Initial handle state is set to an invalid state. Constructor is
			/// hidden from user because there is presently no known scenario where the API
			/// shouldn't be explicitly creating and populating the internal pointer for the user.
			/// </summary>
			DivertHandle();

			/// <summary>
			/// Allow internal construction with the supplied unmanaged handle.
			/// </summary>
			/// <param name="handle">
			/// Unmanaged HANDLE to construct this wrapper around.
			/// </param>
			DivertHandle(HANDLE handle, const bool fromWinDivert);

			/// <summary>
			/// Internal accessor to the unmanaged handle object held by this object.
			/// </summary>
			/// <returns>
			/// The unmanaged HANDLE member, regardless of its validity.
			/// </returns>
			property HANDLE UnmanagedHandle
			{
				HANDLE get();
				void set(HANDLE value);
			}

		private:

			/// <summary>
			/// Privately held HANDLE member. Exposed internally only so that other members of the
			/// library can access it, but it's kept away from the user.
			/// </summary>
			HANDLE m_handle = INVALID_HANDLE_VALUE;

			/// <summary>
			/// Depending on how the handle was created, we need to handle closing it differently.
			/// If this handle was created from a WinDivertOpen(...) call, then it needs to be
			/// closed through the WinDivert API. However, this library allows use of the Ex (async)
			/// WinDivert methods, which will use HANDLE objects created through the WinAPI. Both
			/// handles are wrapped by this same object, so this object must be aware of how to
			/// treat the underlying native handle. The user needs not concern themselves with this.
			/// </summary>
			bool m_fromWinDivert = false;
		};

	} /* namespace Net */
} /* namespace Divert  */
