namespace SurfEngine
{
	public struct Vector4
	{
		public float r, g, b, a;

		public Vector4(float scalar)
		{
			r = scalar;
			g = scalar;
			b = scalar;
			a = scalar;
		}

		public Vector4(float r, float g, float b, float a)
		{
			this.r = r;
			this.g = g;
			this.b = b;
			this.a = a;
		}

        public override string ToString()
        {
			return (string)("[" + r.ToString() + ", " + g.ToString() + ", " + b.ToString() + ", " + a.ToString() + "]");
        }

        public static Vector4 operator +(Vector4 a, Vector4 b)
		{
			return new Vector4(a.r + b.r, a.g + b.g, a.b + b.b, a.a+ b.a);
		}

		public static Vector4 operator *(Vector4 vector, float scalar)
		{
			return new Vector4(vector.r * scalar, vector.g * scalar, vector.b * scalar, vector.a * scalar);
		}

	}
}