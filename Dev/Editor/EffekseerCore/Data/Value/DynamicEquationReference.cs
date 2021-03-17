using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Data.Value
{
	public class DynamicEquationReference : ObjectReference<DynamicEquation>
	{
		public DynamicEquationReference()
		{
			GetHolder += (DynamicEquation e) =>
			{
				return Core.Dynamic.Equations.GetHolder(e);
			};

			GetValueWithIndex += (int ind) =>
			{
				if (0 <= ind && ind < Core.Dynamic.Equations.Values.Count)
				{
					return Core.Dynamic.Equations.Values[ind];
				}
				return null;
			};
		}
	}
}
