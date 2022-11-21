#include <iostream>
#pragma once

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next)
        {

        }
        Type value;
        Node* next_node = nullptr;
    };

    // Шаблон класса «Базовый Итератор».
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator {
        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) :
            node_(node)
        {

        }

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        Node *node_;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept
        {
            node_ = other.node_;
        }

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов
        // (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент
        // списка либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept
        {
            if (node_ == nullptr && rhs.node_ == nullptr)
            {
                return true;
            }

            if (node_ == nullptr || rhs.node_ == nullptr)
            {
                return false;
            }

            return node_->next_node == rhs.node_->next_node;
        }

        // Оператор проверки итераторов на неравенство
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept
        {
            return !operator==(rhs);
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept
        {
            if (node_ == nullptr && rhs.node_ == nullptr)
            {
                return true;
            }

            if (node_ == nullptr || rhs.node_ == nullptr)
            {
                return false;
            }

            return node_->next_node == rhs.node_->next_node;
        }

        // Оператор проверки итераторов на неравенство
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept
        {
            return !operator==(rhs);
        }

        // Оператор прединкремента.
        // После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        BasicIterator& operator++() noexcept
        {
            assert(this->node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента.
        // После его вызова итератор указывает на следующий элемент списка
        // Возвращает прежнее значение итератора
        BasicIterator operator++(int) noexcept
        {
            assert(this->node_ != nullptr);
            BasicIterator to_return(this->node_);
            node_ = node_->next_node;
            return to_return;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        [[nodiscard]] reference operator*() const noexcept
        {
            return node_->value;
        }

        // Операция доступа к члену класса.
        // Возвращает указатель на текущий элемент списка
        // Вызов этого оператора у итератора,
        // не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept
        {
            return &node_->value;
        }
    };

public:

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList()
        : head_(),
          size_(0)
    {

    }

    SingleLinkedList(std::initializer_list<Type> values)
        : head_(),
          size_(0)
    {
        assert(size_ == 0 && head_.next_node == nullptr);
        assign(values);
    }

    SingleLinkedList(const SingleLinkedList& other)
        : head_(),
          size_(0)
    {
        assert(size_ == 0 && head_.next_node == nullptr);
        assign(other.begin(), other.end());
    }

    ~SingleLinkedList()
    {
        Clear();
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs)
    {
        if (this != &rhs)
        {
            SingleLinkedList copy(rhs);
            swap(copy);
        }
        return *this;
    }

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept
    {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept
    {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return size_ == 0;
    }

    void PushFront(const Type& value)
    {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void PopFront() noexcept
    {
        if (IsEmpty() && head_.next_node == nullptr)
        {
            return;
        }

        assert(!IsEmpty() || head_.next_node != nullptr);

        Node *ptr_to_delete = head_.next_node;
        head_.next_node = ptr_to_delete->next_node;
        --size_;
        delete ptr_to_delete;
    }

    void Clear() noexcept
    {
        while (head_.next_node != nullptr)
        {
            PopFront();
        }
    }

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept
    {
        return Iterator{head_.next_node};
    }

    // Возвращает итератор, указывающий на позицию,
    // следующую за последним элементом односвязного списка
    [[nodiscard]] Iterator end() noexcept
    {
        return Iterator{nullptr};
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept
    {
        return cbegin();
    }

    // Возвращает константный итератор, указывающий на позицию,
    // следующую за последним элементом односвязного списка
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept
    {
        return cend();
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept
    {
        return ConstIterator{head_.next_node};
    }

    // Возвращает константный итератор, указывающий на позицию,
    // следующую за последним элементом односвязного списка
    [[nodiscard]] ConstIterator cend() const noexcept
    {
        return ConstIterator{nullptr};
    }

    // Возвращает итератор, указывающий на позицию перед первым
    // элементом односвязного списка.
    [[nodiscard]] Iterator before_begin() noexcept
    {
        return Iterator{&head_};
    }

    // Возвращает константный итератор, указывающий на позицию
    // перед первым элементом односвязного списка.
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept
    {
        return ConstIterator{const_cast<Node *>(&head_)};
    }

    // Возвращает константный итератор, указывающий на позицию
    // перед первым элементом односвязного списка.
    [[nodiscard]] ConstIterator before_begin() const noexcept
    {
        // Реализуйте самостоятельно
        return cbefore_begin();
    }

    /*
        * Вставляет элемент value после элемента, на который указывает pos.
        * Возвращает итератор на вставленный элемент
        * Если при создании элемента будет выброшено исключение,
        * список останется в прежнем состоянии
        */
    Iterator InsertAfter(ConstIterator pos, const Type& value)
    {
        assert(pos.node_ != nullptr);

        Node *ptr_to_insert = new Node(value, pos.node_->next_node);
        pos.node_->next_node = ptr_to_insert;
        ++size_;
        return Iterator{ptr_to_insert};
    }

    /*
        * Удаляет элемент, следующий за pos.
        * Возвращает итератор на элемент, следующий за удалённым
        */
    Iterator EraseAfter(ConstIterator pos) noexcept
    {
        assert(!IsEmpty());

        Node *ptr_to_delete = pos.node_->next_node;
        pos.node_->next_node = ptr_to_delete->next_node;
        --size_;
        delete ptr_to_delete;
        return Iterator{pos.node_->next_node};
    }

private:

    void assign(const std::initializer_list<Type> &values) noexcept
    {
        assign(values.begin(), values.end());
    }

    template <typename InputIterator>
    void assign(InputIterator from, InputIterator to) noexcept
    {
        SingleLinkedList tmp = {};
        Node *last_node_tmp = &tmp.head_;
        for (InputIterator it = from; it != to; ++it)
        {
            Node *copy_node = new Node(*it, nullptr);
            last_node_tmp->next_node = copy_node;
            last_node_tmp = copy_node;
            ++tmp.size_;
        }

        swap(tmp);
    }

    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_ = 0;

};


template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs,
                const SingleLinkedList<Type>& rhs)
{
    return (&lhs == &rhs) ||
            (lhs.GetSize() == rhs.GetSize()
             && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs,
                const SingleLinkedList<Type>& rhs)
{
    return !operator==(lhs, rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs,
               const SingleLinkedList<Type>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs,
               const SingleLinkedList<Type>& rhs)
{
    return !operator<(lhs, rhs) || operator!=(lhs, rhs);
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs,
                const SingleLinkedList<Type>& rhs)
{
    return operator<(lhs, rhs) || operator==(lhs, rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs,
                const SingleLinkedList<Type>& rhs)
{
    return !operator<(lhs, rhs);
}
