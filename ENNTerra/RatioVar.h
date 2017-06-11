#pragma once
#ifndef RATIOVAR_H
#define RATIOVAR_H

namespace ThGkatz
{
	//singleton class for the useage of float variable RATIO across the different files of the project
	class RatioVar
	{
	private:
		float RATIO;
		static RatioVar* var;
		RatioVar(float number =0) {
			RATIO = number;
		}
	public:
		static RatioVar* instance()
		{
			if (!var)
				var = new RatioVar;
			return var;
		}
		void setRATIO(float number)
		{
			RATIO = number;
		}
		float getRATIO()
		{
			return RATIO;
		}
	};
}

#endif // !RATIOVAR_H
