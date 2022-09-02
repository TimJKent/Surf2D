using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SurfEngine
{
	public class Script
	{
		protected Script() { gameObject = new GameObject(0); }

		internal Script(ulong id)
		{
			gameObject = new GameObject(id);
		}

		public readonly GameObject gameObject;
	}
}
