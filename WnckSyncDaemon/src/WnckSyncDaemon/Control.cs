//  
//  Copyright (C) 2009 Canonical Ltd.
// 
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 

using System;
using System.Collections.Generic;
using System.Linq;

using Gnome;
using GLib;

using Wnck;

namespace WnckSyncDaemon
{
	
	
	public class Control : IControl
	{

		#region IControl implementation
		public string DesktopFileForXid (uint id)
		{
			Wnck.Window win = Wnck.Window.Get ((ulong) id);
			
			return WindowMatcher.DesktopFileForWindow (win);
		}
		
		public uint [] XidsForDesktopFile (string filename)
		{
			return WindowMatcher.WindowListForDesktopFile (filename).Select (w => (uint) w.Xid).ToArray ();
		}
		#endregion
		
		public Control ()
		{
		}
	}
}