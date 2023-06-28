/*!               Шаблон базового класса который может иметь только один экземпляр

(с) 2018-sep: steps: synset.com, absolutist.com, qudata.com
*/

#define IMPLEMENTATION_SINGLETONE(ClassName)             \
public:                                                  \
   static ClassName& instance()                          \
   {                                                     \
      static ClassName  inst;                            \
      return inst;                                       \
   }                                                     \
private:                                                 \
   /* закрываем доступ к конструктору по умолчанию */    \
   ClassName() {}                                        \
public:                                                  \
   /* закрываем доступ к конструктору по умолчанию, конструктору копирования и оператору присваивания*/  \
   ClassName(ClassName const&)      = delete;                                                            \
   void operator=(ClassName const&) = delete;                                                            \
private:

