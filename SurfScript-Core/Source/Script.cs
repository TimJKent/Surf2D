using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SurfEngine
{
	public class Script
	{
		protected Script() { GameObject = new GameObject(0); }

		internal Script(ulong id)
		{
			GameObject = new GameObject(id);
		}

		public readonly GameObject GameObject;
	}
}
