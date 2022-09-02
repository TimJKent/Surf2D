using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SurfEngine
{
    public static class Debug
    {
        public static void Log(object t) {
            
            InternalCalls.Debug_Log(t.ToString());
        }

       public static void Warn(object t){
            InternalCalls.Debug_Warn(t.ToString());
        }

        public static void Error(object t) { 
            InternalCalls.Debug_Error(t.ToString());
        }
    }
}
