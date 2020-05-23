#ifndef UTILS_AST
#define UTILS_AST

#include <memory>

namespace spc
{
    class BaseNode;

    template<typename T, typename U>
    inline typename std::enable_if<std::is_base_of<BaseNode, U>::value && std::is_base_of<BaseNode, T>::value, bool>::type 
    is_ptr_of(const std::shared_ptr<U> &ptr)
    {
        return dynamic_cast<T *>(ptr.get()) != nullptr;
    }

    template<typename T, typename U>
    inline typename std::enable_if<std::is_base_of<BaseNode, U>::value && std::is_base_of<BaseNode, T>::value, std::shared_ptr<T>>::type
    cast_node(const std::shared_ptr<U> &ptr)
    {
        return std::dynamic_pointer_cast<T>(ptr);
    }
    
} // namespace spc


#endif
