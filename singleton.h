/**
 * @file singleton.h
 * @brief 单例模式封装
 * @author shawn
 * @date 2024-05-18
 */
namespace coro {

/**
 * @brief 单例模式封装类
 * @details T 类型
 *          X 为了创造多个实例对应的Tag
 *          N 同一个Tag创造多个实例索引
 */
template <class T, class x = void, int N = 0>
class Singleton {
   public:
    /**
    @brief 返回单例裸指针
     */
    static T* GetInstance() {
        static T v;
        return &v;
    }
};
}  // namespace coro