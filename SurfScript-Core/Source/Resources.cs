using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SurfEngine
{

    public abstract class Resource
    {
        public string Path { get; internal set; }
    }

    public class Sprite : Resource {
        public Sprite()
        {
            this.Path = "";
        }

        internal Sprite(string path) {
            this.Path = path;
        }
    }


    public static class Resources
   {
		public static T Load<T>(string path) where T : Resource, new() {

        
            T resource = new T() { Path = InternalCalls.Resources_GetPath(path)};
            return resource;
		}
   }
}
