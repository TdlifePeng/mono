// THIS FILE AUTOMATICALLY GENERATED BY xpidl2cs.pl
// EDITING IS PROBABLY UNWISE
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Copyright (c) 2007, 2008 Novell, Inc.
//
// Authors:
//	Andreia Gaita (avidigal@novell.com)
//

using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Text;

namespace Mono.Mozilla {

	[Guid ("78650582-4e93-4b60-8e85-26ebd3eb14ca")]
	[InterfaceType (ComInterfaceType.InterfaceIsIUnknown)]
	[ComImport ()]
	internal interface nsIProperties {

#region nsIProperties
		[PreserveSigAttribute]
		[MethodImpl (MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
		int get ([MarshalAs (UnmanagedType.LPStr) ]  string prop,
				[MarshalAs (UnmanagedType.LPStruct) ]  Guid iid,
				out IntPtr result);

		[PreserveSigAttribute]
		[MethodImpl (MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
		int set ([MarshalAs (UnmanagedType.LPStr) ]  string prop,
				[MarshalAs (UnmanagedType.Interface) ]  IntPtr value);

		[PreserveSigAttribute]
		[MethodImpl (MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
		int has ([MarshalAs (UnmanagedType.LPStr) ]  string prop,
				out bool ret);

		[PreserveSigAttribute]
		[MethodImpl (MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
		int undefine ([MarshalAs (UnmanagedType.LPStr) ]  string prop);

		[PreserveSigAttribute]
		[MethodImpl (MethodImplOptions.InternalCall, MethodCodeType = MethodCodeType.Runtime)]
		int getKeys (out UInt32 count,
				[MarshalAs (UnmanagedType.LPStr) ] out string[] keys);

#endregion
	}


	internal class nsProperties {
		public static nsIProperties GetProxy (Mono.WebBrowser.IWebBrowser control, nsIProperties obj)
		{
			object o = Base.GetProxyForObject (control, typeof(nsIProperties).GUID, obj);
			return o as nsIProperties;
		}
	}
}
#if example

using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Text;

	internal class Properties : nsIProperties {

#region nsIProperties
		int nsIProperties.get ([MarshalAs (UnmanagedType.LPStr) ]  string prop,
				[MarshalAs (UnmanagedType.LPStruct) ]  Guid iid,
				out IntPtr result)
		{
			return ;
		}



		int nsIProperties.set ([MarshalAs (UnmanagedType.LPStr) ]  string prop,
				[MarshalAs (UnmanagedType.Interface) ]  IntPtr value)
		{
			return ;
		}



		int nsIProperties.has ([MarshalAs (UnmanagedType.LPStr) ]  string prop,
				out bool ret)
		{
			return ;
		}



		int nsIProperties.undefine ([MarshalAs (UnmanagedType.LPStr) ]  string prop)
		{
			return ;
		}



		int nsIProperties.getKeys (out UInt32 count,
				[MarshalAs (UnmanagedType.LPStr) ] out string[] keys)
		{
			return ;
		}



#endregion
	}
#endif
