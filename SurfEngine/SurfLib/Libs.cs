using System;
using System.Runtime.CompilerServices;

namespace SurfEngine {

    public class Script {

        public void SetGameObject(string uuid) {
            game_object = new GameObject(uuid);
        }

        public GameObject game_object;
    }

    public abstract class Component {

        public static bool operator true(Component a)
        {
            if (a == null) { return false; }
            return a.uuid > 0;
        }

        public static bool operator false(Component a)
        {
            if (a == null) { return true; }
            return a.uuid == 0;
        }

        public Component()
        {
            this.uuid = 0;
        }

        public void SetUUID(ulong uuid) {
                this.uuid = uuid;    
        }

        public abstract string GetComponentType();

        protected ulong uuid = (ulong)0;

    }

    public class GameObject {

        public GameObject() { 
        
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetGameObjectByNameImpl(string name);
        public static GameObject GetGameObjectByName(string name) {
            return new GameObject(GetGameObjectByNameImpl(name));
        }

        public GameObject(string uuid) {
            this.uuid = UInt64.Parse(uuid);
        }

        public static bool operator true(GameObject a) {
            if (a == null) { return false; }
            return a.uuid > 0;
        }

        public static bool operator false(GameObject a)
        {
            if (a == null) { return true; }
            return a.uuid == 0;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetNameImpl(string uuid);
        public string GetName() { return GetNameImpl(this.uuid.ToString()); }


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool HasComponentImpl(string obj_uuid, string component_name);
        public bool HasComponent<T>() where T : Component, new()
        {
            T component = new T();
            return HasComponentImpl(this.uuid.ToString(), component.GetComponentType());
        }

        public T GetComponent<T>() where T : Component, new()
        {
            
            T component = new T();
            if (HasComponent<T>()) { 
                component.SetUUID(this.uuid);
            }
            return component;
        }

        private ulong uuid = (ulong)0;
    }

    public class Vector3 {
        public Vector3() { 
            x = 0; y = 0; z = 0; 
        }

        public Vector3(double x, double y, double z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public override string ToString() {
            return "Vector3: [" + x + ", " + y + ", " + z + "]";
        }

        public static Vector3 Lerp(Vector3 Start, Vector3 Finish, double interpolation) {
            Vector3 newPos = new Vector3();
            newPos.x = Start.x + ((Finish.x - Start.x) * interpolation);
            newPos.y = Start.y + ((Finish.y - Start.y) * interpolation);
            newPos.z = Start.z + ((Finish.z - Start.z) * interpolation);
            return newPos;
        }

        public double x = 0;
        public double y = 0;
        public double z = 0;
    }

    public class Color
    {
        public Color()
        {
            r = 0; g = 0; b = 0; a = 0;
        }

        public Color(double r, double g, double b)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = 1.0;
        }

        public Color(double r, double g, double b, double a)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }

        public override string ToString()
        {
            return "Color: [" + r + ", " + g + ", " + b + ", " + a + "]";
        }

        public static Color Lerp(Color Start, Color Finish, double interpolation)
        {
            Color newColor = new Color();
            newColor.r = Start.r + ((Finish.r - Start.r) * interpolation);
            newColor.g = Start.g + ((Finish.g - Start.g) * interpolation);
            newColor.b = Start.b + ((Finish.b - Start.b) * interpolation);
            newColor.a = Start.a + ((Finish.a - Start.a) * interpolation);
            return newColor;
        }

        public double r = 0;
        public double g = 0;
        public double b = 0;
        public double a = 0;
    }

    public class Transform : Component {

        public override string GetComponentType() {
            return "Transform";
        }

        //POSITION
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double[] GetPositionImpl(string uuid);
        public Vector3 GetPosition() {
            double[] pos = GetPositionImpl(this.uuid.ToString());
            return new Vector3(pos[0], pos[1], pos[2]);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPositionImpl(string uuid, double x, double y, double z);
        public void SetPosition(Vector3 new_pos) {
            SetPositionImpl(this.uuid.ToString(), new_pos.x, new_pos.y, new_pos.z);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void TranslateXImpl(string uuid, double x);
        public void TranslateX(double x)
        {
            TranslateXImpl(this.uuid.ToString(), x);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void TranslateYImpl(string uuid, double y);
        public void TranslateY(double y)
        {
            TranslateYImpl(this.uuid.ToString(), y);
        }

        //ROTATION
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double GetRotationImpl(string uuid);
        public double GetRotation()
        {
            return GetRotationImpl(this.uuid.ToString());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRotationImpl(string uuid, double new_rot);
        public void SetRotation(double new_rot)
        {
            SetRotationImpl(this.uuid.ToString(),new_rot);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RotateImpl(string uuid, double z);
        public void Rotate(double z)
        {
            RotateImpl(this.uuid.ToString(), z);
        }

        //Scale
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double[] GetScaleImpl(string uuid);
        public Vector3 GetScale()
        {
            double[] scale = GetScaleImpl(this.uuid.ToString());
            return new Vector3(scale[0], scale[1], scale[2]);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetScaleImpl(string uuid, double x, double y, double z);
        public void SetScale(Vector3 new_scale)
        {
            SetScaleImpl(this.uuid.ToString(), new_scale.x, new_scale.y, new_scale.z);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ScaleXImpl(string uuid, double x);
        public void ScaleX(double x)
        {
            ScaleXImpl(this.uuid.ToString(),x);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ScaleYImpl(string uuid, double y);
        public void ScaleY(double y)
        {
            ScaleYImpl(this.uuid.ToString(), y);
        }
    }

    public class SpriteRenderer : Component
    {

        public override string GetComponentType()
        {
            return "SpriteRenderer";
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void FlipXImpl(string uuid, bool flipX);
        public void FlipX(bool flipX)
        {
            FlipXImpl(this.uuid.ToString(), flipX);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double[] GetColorImpl(string uuid);
        public Color GetColor()
        {
            double[] color = GetColorImpl(this.uuid.ToString());
            return new Color(color[0], color[1], color[2], color[3]);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetColorImpl(string uuid, double r, double g, double b, double a);
        public void SetColor(Color color)
        {
            SetColorImpl(this.uuid.ToString(),color.r,color.g,color.b,color.a);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint GetLayerImpl(string uuid);
        public uint GetLayer()
        {
           return GetLayerImpl(this.uuid.ToString());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetLayerImpl(string uuid, uint layer);
        public void SetLayer(uint layer)
        {
            SetLayerImpl(this.uuid.ToString(), layer);
        }
    }

    public enum ForceType { 
        Force = 0, Acceleration, Impulse
    }

    public class Rigidbody : Component
    {

        public override string GetComponentType()
        {
            return "Rigidbody";
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddForceImpl(string uuid, double x, double y, double z, int type);
        public void AddForce(Vector3 force, ForceType mode = ForceType.Force)
        {
            int type = 0;
            switch (mode) {
                case ForceType.Force:           type = 0;  break;
                case ForceType.Acceleration:    type = 1;  break;
                case ForceType.Impulse:         type = 2;  break;
            }
            AddForceImpl(this.uuid.ToString(), force.x, force.y, force.z, type);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetVelocityImpl(string uuid, double x, double y, double z);
        public void SetVelocity(Vector3 force)
        {
            SetVelocityImpl(this.uuid.ToString(), force.x, force.y, force.z);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddTorqueImpl(string uuid, double torque, int type);
        public void AddTorque(double torque, ForceType mode = ForceType.Force)
        {
            int type = 0;
            switch (mode)
            {
                case ForceType.Force: type = 0; break;
                case ForceType.Acceleration: type = 1; break;
                case ForceType.Impulse: type = 2; break;
            }
            AddTorqueImpl(this.uuid.ToString(), torque, type);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTorqueImpl(string uuid, double torque);
        public void SetTorque(double torque)
        {
            SetTorqueImpl(this.uuid.ToString(), torque);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double[] GetVelocityImpl(string uuid);
        public Vector3 GetVelocity()
        {
            double[] velocity = GetVelocityImpl(this.uuid.ToString());
            return new Vector3(velocity[0], velocity[1], velocity[2]);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double GetTorqueImpl(string uuid);
        public double GetTorque()
        {
           return GetTorqueImpl(this.uuid.ToString());
        }
    }

    public class BoxCollider : Component
    {

        public override string GetComponentType()
        {
            return "BoxCollider";
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double[] GetSizeImpl(string uuid);
        public Vector3 GetSize()
        {
            double[] size = GetSizeImpl(this.uuid.ToString());
            return new Vector3(size[0], size[1], size[2]);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double[] GetOffsetImpl(string uuid);
        public Vector3 GetOffset()
        {
            double[] off = GetOffsetImpl(this.uuid.ToString());
            return new Vector3(off[0], off[1], off[2]);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetSizeImpl(string uuid, double x, double y);
        public void SetSize(Vector3 size)
        {
            SetSizeImpl(uuid.ToString(), size.x, size.y);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetOffsetImpl(string uuid, double x, double y);
        public void SetOffset(Vector3 offset)
        {
            SetOffsetImpl(uuid.ToString(), offset.x, offset.y);
        }
    }


    public class CircleCollider : Component
    {
        public override string GetComponentType()
        {
            return "CircleCollider";
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double GetRadiusImpl(string uuid);
        public double GetRadius()
        {
            return GetRadiusImpl(this.uuid.ToString());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double[] GetOffsetImpl(string uuid);
        public Vector3 GetOffset()
        {
            double[] off = GetOffsetImpl(this.uuid.ToString());
            return new Vector3(off[0], off[1], off[2]);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRadiusImpl(string uuid, double radius);
        public void SetRadius(float radius)
        {
            SetRadiusImpl(uuid.ToString(), radius);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetOffsetImpl(string uuid, double x, double y);
        public void SetOffset(Vector3 offset)
        {
            SetOffsetImpl(uuid.ToString(), offset.x, offset.y);
        }
    }


    public class Camera :Component{
        public override string GetComponentType()
        {
            return "Camera";
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern double GetSizeImpl(string uuid);
        public double GetSize()
        {
            return GetSizeImpl(this.uuid.ToString());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetSizeImpl(string uuid, double size);
        public void SetSize(double size)
        {
            SetSizeImpl(this.uuid.ToString(), size);
        }
    }

    public class Animation : Component{
        public override string GetComponentType()
        {
            return "Animation";
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint GetFrameImpl(string uuid);
        public uint GetFrame()
        {
            return GetFrameImpl(this.uuid.ToString());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFrameImpl(string uuid, uint frame);
        public void SetFrame(uint frame)
        {
            SetFrameImpl(this.uuid.ToString(), frame);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void PlayImpl(string uuid);
        public void Play()
        {
            PlayImpl(this.uuid.ToString());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void StopImpl(string uuid);
        public void Stop()
        {
            StopImpl(this.uuid.ToString());
        }
    }

    public class KeyCode
    {
        public static int SPACE = 32                ;
        public static int APOSTROPHE = 39           ;
        public static int COMMA = 44                ;
        public static int MINUS = 45                ;
        public static int PERIOD = 46               ;
        public static int SLASH = 47                ;
        public static int NUM_0 = 48                ;
        public static int NUM_1 = 49                ;
        public static int NUM_2 = 50                ;
        public static int NUM_3 = 51                ;
        public static int NUM_4 = 52                ;
        public static int NUM_5 = 53                ;
        public static int NUM_6 = 54                ;
        public static int NUM_7 = 55                ;
        public static int NUM_8 = 56                ;
        public static int NUM_9 = 57                ;
        public static int SEMICOLON = 59            ;
        public static int EQUAL = 61                ;
        public static int A = 65                    ;
        public static int B = 66                    ;
        public static int C = 67                    ;
        public static int D = 68                    ;
        public static int E = 69                    ;
        public static int F = 70                    ;
        public static int G = 71                    ;
        public static int H = 72                    ;
        public static int I = 73                    ;
        public static int J = 74                    ;
        public static int K = 75                    ;
        public static int L = 76                    ;
        public static int M = 77                    ;
        public static int N = 78                    ;
        public static int O = 79                    ;
        public static int P = 80                    ;
        public static int Q = 81                    ;
        public static int R = 82                    ;
        public static int S = 83                    ;
        public static int T = 84                    ;
        public static int U = 85                    ;
        public static int V = 86                    ;
        public static int W = 87                    ;
        public static int X = 88                    ;
        public static int Y = 89                    ;
        public static int Z = 90                    ;
        public static int LEFT_BRACKET = 91         ;
        public static int BACKSLASH = 92            ;
        public static int RIGHT_BRACKET = 93        ;
        public static int GRAVE_ACCENT = 96         ;
        public static int WORLD_1 = 161             ;
        public static int WORLD_2 = 162             ;
        public static int ESCAPE = 256              ;
        public static int ENTER = 257               ;
        public static int TAB = 258                 ;
        public static int BACKSPACE = 259           ;
        public static int INSERT = 260              ;
        public static int DELETE = 261              ;
        public static int RIGHT = 262               ;
        public static int LEFT = 263                ;
        public static int DOWN = 264                ;
        public static int UP = 265                  ;
        public static int PAGE_UP = 266             ;
        public static int PAGE_DOWN = 267           ;
        public static int HOME = 268                ;
        public static int END = 269                 ;
        public static int CAPS_LOCK = 280           ;
        public static int SCROLL_LOCK = 281         ;
        public static int NUM_LOCK = 282            ;
        public static int PRINT_SCREEN = 283        ;
        public static int PAUSE = 284               ;
        public static int F1 = 290                  ;
        public static int F2 = 291                  ;
        public static int F3 = 292                  ;
        public static int F4 = 293                  ;
        public static int F5 = 294                  ;
        public static int F6 = 295                  ;
        public static int F7 = 296                  ;
        public static int F8 = 297                  ;
        public static int F9 = 298                  ;
        public static int F10 = 299                 ;
        public static int F11 = 300                 ;
        public static int F12 = 301                 ;
        public static int F13 = 302                 ;
        public static int F14 = 303                 ;
        public static int F15 = 304                 ;
        public static int F16 = 305                 ;
        public static int F17 = 306                 ;
        public static int F18 = 307                 ;
        public static int F19 = 308                 ;
        public static int F20 = 309                 ;
        public static int F21 = 310                 ;
        public static int F22 = 311                 ;
        public static int F23 = 312                 ;
        public static int F24 = 313                 ;
        public static int F25 = 314                 ;
        public static int KP_0 = 320                ;
        public static int KP_1 = 321                ;
        public static int KP_2 = 322                ;
        public static int KP_3 = 323                ;
        public static int KP_4 = 324                ;
        public static int KP_5 = 325                ;
        public static int KP_6 = 326                ;
        public static int KP_7 = 327                ;
        public static int KP_8 = 328                ;
        public static int KP_9 = 329                ;
        public static int KP_DECIMAL = 330          ;
        public static int KP_DIVIDE = 331           ;
        public static int KP_MULTIPLY = 332         ;
        public static int KP_SUBTRACT = 333         ;
        public static int KP_ADD = 334              ;
        public static int KP_ENTER = 335            ;
        public static int KP_EQUAL = 336            ;
        public static int LEFT_SHIFT = 340          ;
        public static int LEFT_CONTROL = 341        ;
        public static int LEFT_ALT = 342            ;
        public static int LEFT_SUPER = 343          ;
        public static int RIGHT_SHIFT = 344         ;
        public static int RIGHT_CONTROL = 345       ;
        public static int RIGHT_ALT = 346           ;
        public static int RIGHT_SUPER = 347         ;
        public static int MENU = 348                ;
    }

    public class Input {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static bool GetKeyDownImpl(int keycode);
        public static bool GetKeyDown(int keycode) { return GetKeyDownImpl(keycode); }
    }


    public class Debug{
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void LogImpl(string msg);

        public static void Log<T>(T msg) { LogImpl(msg.ToString()); }

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void WarnImpl(string msg);
        public static void Warn<T>(T msg) { WarnImpl(msg.ToString()); }

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void ErrorImpl(string msg);
        public static void Error<T>(T msg) { ErrorImpl(msg.ToString()); }
    }
}


