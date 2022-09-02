using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SurfEngine
{
    public static class Input
    {
        public static bool IsKeyDown(KeyCode keyCode) {
            return InternalCalls.Input_IsKeyDown(keyCode);
        }

        public static bool IsKeyUp(KeyCode keyCode)
        {
            return InternalCalls.Input_IsKeyUp(keyCode);

        }
    }
}
