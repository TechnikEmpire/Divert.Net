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

#include <cstdint>
#include <windivert.h>

namespace Divert
{
	namespace Net
	{
		/// <summary>
		/// Used to determine the direction of captured packets.
		/// </summary>
		public enum DivertDirection
		{
			Outbound = 0,
			Inbound = 1
		};

		/// <summary>
		/// The Address is supplied to Send/Recv methods, where information such as the
		/// interface, sub-interface and the direction of the packet captured will be stored in this
		/// object in the event of a successful call.
		/// </summary>
		public ref class Address
		{

		public:

			/// <summary>
			/// Default constructor for the Address object.
			/// </summary>
			Address();

			/// <summary>
			/// Destructor, invokes finalizer as per docs here
			/// https://msdn.microsoft.com/library/ms177197(v=vs.100).aspx.
			/// </summary>
			~Address();

			/// <summary>
			/// Finalizer for releasing unmanaged resources.
			/// </summary>
			!Address();

			/// <summary>
			/// The interface index on which the packet arrived (for inbound packets), or is to be
			/// sent (for outbound packets).Taken from
			/// https://reqrypt.org/windivert-doc.html#divert_address.
			/// </summary>
			property uint32_t InterfaceIndex
			{
				uint32_t get();
				void set(uint32_t value);
			}

			// <summary>
			/// The sub-interface index for IfIdx. Taken from
			/// https://reqrypt.org/windivert-doc.html#divert_address.
			/// </summary>
			property uint32_t SubInterfaceIndex
			{
				uint32_t get();
				void set(uint32_t value);
			}

			/// <summary>
			/// The packet's direction.
			/// </summary>
			property DivertDirection Direction
			{
				DivertDirection get();
				void set(DivertDirection value);
			}

		internal:

			/// <summary>
			/// Allow internal construction with the supplied unmanaged address.
			/// </summary>
			/// <param name="address">
			/// Unmanaged PWINDIVERT_ADDRESS to construct this wrapper around.
			/// </param>
			Address(PWINDIVERT_ADDRESS address);

			/// <summary>
			/// Internal accessor to the unmanaged PWINDIVERT_ADDRESS object held by this object. 
			/// </summary>
			/// <returns>
			/// The unmanaged PWINDIVERT_ADDRESS member. Should always be a valid pointer. 
			/// </returns>
			const PWINDIVERT_ADDRESS GetUnmanagedAddress();

		private:

			/// <summary>
			/// Privately held PWINDIVERT_ADDRESS member. Exposed internally only so that other
			/// members of the library can access it, but it's kept away from the user.
			/// </summary>
			PWINDIVERT_ADDRESS m_address;			

		};

	} /* namespace Net */
} /* namespace Divert  */
