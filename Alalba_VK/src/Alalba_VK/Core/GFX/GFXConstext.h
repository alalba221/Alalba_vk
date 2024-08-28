#pragma once
namespace Alalba
{
	class Window;
	
	class GFXConstext
	{
	public:
		GFXConstext(const GFXConstext&) = delete;
		GFXConstext& operator=(const GFXConstext&) = delete;
		virtual ~GFXConstext() = default;

		static std::unique_ptr<GFXConstext> Create(Window* window);

	protected:
		GFXConstext() = default;
	};
}
