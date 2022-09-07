using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SurfEngine
{
    public abstract class Component
    {
        public GameObject GameObject { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector2 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(GameObject.ID, out Vector2 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(GameObject.ID, ref value);
            }
        }

        public float Rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(GameObject.ID, out float rotation);
                return rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(GameObject.ID, ref value);
            }
        }

        public Vector2 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(GameObject.ID, out Vector2 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(GameObject.ID, ref value);
            }
        }

        public void TranslateX(float amount) {
            this.Translation = new Vector2(this.Translation.x + amount, this.Translation.y);
        }

        public void TranslateY(float amount){
            this.Translation = new Vector2(this.Translation.x, this.Translation.y + amount);
        }

        public void Rotate(float amount)
        {
            this.Rotation = this.Rotation + amount;
        }

        public void ScaleX(float amount)
        {
            this.Scale = new Vector2(this.Scale.x + amount, this.Scale.y );
        }

        public void ScaleY(float amount)
        {
            this.Scale = new Vector2(this.Scale.x, this.Scale.y + amount);
        }

        public bool HasParent()
        {
            return InternalCalls.TransformComponent_HasParent(GameObject.ID);
        }

        public TransformComponent GetParent() {
            if (!HasParent()) { return null; }
            ulong parentId;
            InternalCalls.TransformComponent_GetParent(GameObject.ID, out parentId);
            return new GameObject(parentId).GetComponent<TransformComponent>();
        }

        public void SetParent(TransformComponent tc)
        {
            ulong parentId = tc.GameObject.ID;
            InternalCalls.TransformComponent_SetParent(GameObject.ID, ref parentId);
        }

        public void SetParent(GameObject gameObject)
        {
            ulong parentId = gameObject.ID;
            InternalCalls.TransformComponent_SetParent(GameObject.ID, ref parentId);
        }

        //public TransformComponent[] children
        //{
        //    get
        //    {
        //        InternalCalls.TransformComponent_GetChildren(GameObject.ID, out ulong parent);
        //        return parent;
        //    }
        //    set
        //    {
        //        InternalCalls.TransformComponent_SetChildren(GameObject.ID, ref ulong parent);
        //    }
        //}
    }         
                
    public class ScriptComponent : Component
    {

    }

    public class SpriteRendererComponent : Component
    {
        public Vector4 Color {
            get
            {
                InternalCalls.SpriteRendererComponent_GetColor(GameObject.ID, out Vector4 color);
                return color;
            }
            set {
                InternalCalls.SpriteRendererComponent_SetColor(GameObject.ID, ref value);
            }
        }

        public int Layer
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetLayer(GameObject.ID, out int layer);
                return layer;
            }
            set
            {
                InternalCalls.SpriteRendererComponent_SetLayer(GameObject.ID, ref value);
            }
        }

        public bool FlipX
        {
            get
            {
                InternalCalls.SpriteRendererComponent_GetFlipX(GameObject.ID, out bool flipX);
                return flipX;
            }
            set
            {
                InternalCalls.SpriteRendererComponent_SetFlipX(GameObject.ID, ref value);
            }
        }

        public Sprite Sprite
        {
            get {
                ;
                return new Sprite(
                    InternalCalls.SpriteRendererComponent_GetSprite(GameObject.ID)
                    );
            }
            set {
                string path = value.Path;
                InternalCalls.SpriteRendererComponent_SetSprite(GameObject.ID, path);
            }
        }
    }

    public class AnimationComponent : Component
    {

    }

    public class CameraComponent : Component
    {

    }

    public class RigidbodyComponent : Component
    {

    }

    public class BoxColliderComponent : Component
    {

    }

    public class CircleColliderComponent : Component
    {

    }
}