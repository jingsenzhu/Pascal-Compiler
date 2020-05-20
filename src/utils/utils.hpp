#ifndef UTILS_AST
#define UTILS_AST

#include <memory>

namespace spc
{
    class BaseNode;

    template<typename T>
    inline bool is_ptr_of(const std::shared_ptr<BaseNode> &ptr)
    {
        assert(std::is_base_of<BaseNode, T>::value);
        return dynamic_cast<T *>(ptr.get()) != nullptr;
    }
    
} // namespace spc


#endif