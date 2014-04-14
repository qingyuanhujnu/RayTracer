#ifndef AVERAGE_HPP
#define AVERAGE_HPP

template <class Type>
class Average
{
public:
	Average ();
	~Average ();

	void	Add (const Type& newVal);
	Type	Get () const;

private:
	Type	val;
	int		count;
};

template <class Type>
Average<Type>::Average () :
	val (Type ()),
	count (0)
{

}

template <class Type>
Average<Type>::~Average ()
{

}

template <class Type>
void Average<Type>::Add (const Type& newVal)
{
	val = val + newVal;
	count = count + 1;
}

template <class Type>
Type Average<Type>::Get () const
{
	return val / count;
}

#endif
