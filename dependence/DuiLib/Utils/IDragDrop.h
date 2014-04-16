#ifndef IDRAGDROP_HPP
#define IDRAGDROP_HPP

namespace DuiLib
{

	class UILIB_API IDragDrop
	{
	public:
		virtual BOOL IsDropSource() = 0;
		virtual BOOL IsDropTarget() = 0;
		virtual BOOL CanDragDrop() = 0;
		virtual BOOL IsShareDragImage() = 0;
	};
}

#endif 
