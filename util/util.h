#ifndef _UTIL_H
#define _UTIL_H


/*
 * 
 *      "If you lie to your compiler, it will get its revenge."
 *
 *                                      -- Henry Spencer
 *
 *
 */


/******************************************************************************
 * COMPILE-TIME CHECKS AND BALANCES
 ******************************************************************************/

#define __ENABLE_BUILD_BUGS

/**
 * __ENABLE_BUILD_BUGS 
 * ```````````````````
 * Provide compile-time reminders, diagnostics, warnings/errors
 * 
 * USAGE
 * Define the macro __ENABLE_BUILD_BUGS 
 */

#ifndef __ENABLE_BUILD_BUGS /* __ENABLE_BUILD_BUGS is not defined */

#ifndef BUILD_BUG_ON_NOT_POWER_OF_2
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)
#endif
#ifndef BUILD_BUG_ON_ZERO
#define BUILD_BUG_ON_ZERO(e)
#endif
#ifndef BUILD_BUG_ON_NULL
#define BUILD_BUG_ON_NULL(e)
#endif
#ifndef BUILD_BUG_ON_INVALID
#define BUILD_BUG_ON_INVALID(e)
#endif
#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(condition)
#endif
#ifndef BUILD_BUG
#define BUILD_BUG()
#endif
#ifndef BUILD_ASSERT
#define BUILD_ASSERT(cond)
#endif
#ifndef BUILD_ASSERT_OR_ZERO
#define BUILD_ASSERT_OR_ZERO(cond)
#endif

#else /* __ENABLE_BUILD_BUGS is defined */


/**
 * BUILD_BUG_ON 
 * ````````````
 * Break compile if some condition is true.
 *
 * @condition: the condition which the compiler should know is false.
 *
 * USAGE
 *
 * If you have some code which relies on certain constants 
 * being equal, or other compile-time-evaluated conditions, 
 * you can use BUILD_BUG_ON to detect if someone changes it.
 *
 * The implementation uses gcc's reluctance to create a negative 
 * array, but gcc (as of 4.4) only emits that error for obvious 
 * cases (eg. not arguments to inline functions). So as a fallback 
 * we use the optimizer; if it can't prove the condition is false, 
 * it will cause a link error on the undefined 
 *
 *      "__build_bug_on_failed".  
 *
 * This error message can be harder to track down though, hence the 
 * two different methods.
 */
extern int __build_bug_on_failed;
#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(condition)					\
	do {							\
		((void)sizeof(char[1 - 2*!!(condition)]));	\
		if (condition) __build_bug_on_failed = 1;	\
	} while(0)
#endif




/**
 * BUILD_BUG 
 * `````````
 * Break the compile if called.
 *
 * If you have some code that you expect the compiler 
 * to eliminate at build time, you should use BUILD_BUG 
 * to detect if it is, in fact, unexpectedly used.
 */
#ifndef BUILD_BUG
#define BUILD_BUG()						\
	do {							\
		extern void __build_bug_failed(void)		\
		__linktime_error("BUILD_BUG failed");	        \
		__build_bug_failed();				\
	} while (0)
#endif




/** 
 * BUILD_BUG_ON_NOT_POWER_OF_2
 * ```````````````````````````
 * Force a compilation error if a constant expression is 
 * not a power of 2 
 *
 * @n: Numeric constant to be checked.
 */
#ifndef BUILD_BUG_ON_NOT_POWER_OF_2
#define BUILD_BUG_ON_NOT_POWER_OF_2(n) \
	BUILD_BUG_ON((n) == 0 || (((n) & ((n) - 1)) != 0))
#endif




/** 
 * BUILD_BUG_ON_ZERO / NULL 
 * ````````````````````````
 * @e: Condition (expression).
 *
 * Force a compilation error if condition is true, but also 
 * produce a result (of value 0 and type size_t), so the 
 * expression can be used e.g. in a structure initializer 
 * (or where-ever else comma expressions aren't permitted). 
 */
#ifndef BUILD_BUG_ON_ZERO
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
#endif
#ifndef BUILD_BUG_ON_NULL
#define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))
#endif




/**
 * BUILD_BUG_ON_INVALID 
 * ````````````````````
 * @e: Condition (expression)
 *
 * Permits the compiler to check the validity of the expression, 
 * but avoids the generation of any code, even if that expression 
 * has side-effects.
 */
#ifndef BUILD_BUG_ON_INVALID
#define BUILD_BUG_ON_INVALID(e) ((void)(sizeof((__force long)(e))))
#endif



/**
 * BUILD_ASSERT 
 * ````````````
 * Assert a build-time dependency.
 *
 * @cond: the compile-time condition which must be true.
 *
 * Your compile will fail if the condition isn't true, or can't 
 * be evaluated by the compiler. This can only be used within a 
 * function.
 *
 * USAGE 
 *
 * #include <stddef.h>
 * static char *foo_to_char(struct foo *foo)
 * {
 *      // This code needs string to be at start of foo.
 *      BUILD_ASSERT(offsetof(struct foo, string) == 0);
 *      return (char *)foo;
 * }
 */
#ifndef BUILD_ASSERT
#define BUILD_ASSERT(cond) do { (void) sizeof(char [1 - 2*!(cond)]); } while(0)
#endif

/**
 * BUILD_ASSERT_OR_ZERO 
 * ````````````````````
 * Assert a build-time dependency, as an expression.
 *
 * @cond: the compile-time condition which must be true.
 *
 * Your compile will fail if the condition isn't true, or can't be evaluated
 * by the compiler. This can be used in an expression: its value is "0".
 *
 * USAGE 
 * #define foo_to_char(foo) \
 *         ((char *)(foo) \
 *         + BUILD_ASSERT_OR_ZERO(offsetof(struct foo, string) == 0))
 */
#ifndef BUILD_ASSERT_OR_ZERO
#define BUILD_ASSERT_OR_ZERO(cond) (sizeof(char [1 - 2*!(cond)]) - 1)
#endif




#endif	/* __ENABLE_BUILD_BUGS */




/******************************************************************************
 * TYPE CHECKING: "GOOD ENOUGH"
 ******************************************************************************/

/**
 * check_type 
 * ``````````
 * Issue a warning or build failure if type is not correct.
 *
 * @expr: the expression whose type we should check (not evaluated).
 * @type: the exact type we expect the expression to be.
 *
 * NOTES
 * This macro is usually used within other macros to try to ensure 
 * that a macro argument is of the expected type. No type promotion 
 * of the expression is done: an unsigned int is not the same as an 
 * int!
 *
 * check_type() always evaluates to 0.
 *
 * If your compiler does not support typeof, then the best we can do is fail
 * to compile if the sizes of the types are unequal (a less complete check).
 *
 * USAGE 
 * They should always pass a 64-bit value to _set_some_value!
 *
 * #define set_some_value(expr) \
 *         _set_some_value((check_type((expr), uint64_t), (expr)))
 */
#if HAVE_TYPEOF
#define check_type(expr, type) \
        ((typeof(expr) *)0 != (type *)0)
#else /* HAVE_TYPEOF */
/* 
 * Without typeof, we can only test the sizes
 * to try and guess if the types match.
 */
#define check_type(expr, type) \
        BUILD_ASSERT_OR_ZERO(sizeof(expr) == sizeof(type))

#endif /* HAVE_TYPEOF */


/**
 * check_types_match 
 * `````````````````
 * Issue a warning or build failure if types are not same.
 *
 * @expr1: the first expression (not evaluated).
 * @expr2: the second expression (not evaluated).
 *
 * This macro is usually used within other macros to try to ensure 
 * that arguments are of identical types. No type promotion of the 
 * expressions is done: an unsigned int is not the same as an int!
 *
 * check_types_match() always evaluates to 0.
 *
 * If your compiler does not support typeof, then the best we can do 
 * is fail to compile if the sizes of the types are unequal (a less 
 * complete check).
 *
 * USAGE
 * // Do subtraction to get to enclosing type, but make sure that
 * // pointer is of correct type for that member.
 * #define container_of(mbr_ptr, encl_type, mbr) \
 *         (check_types_match((mbr_ptr), &((encl_type *)0)->mbr), \
 *         ((encl_type *) \
 *         ((char *)(mbr_ptr) - offsetof(enclosing_type, mbr))))
 */
#if HAVE_TYPEOF
#define check_types_match(expr1, expr2) \
        ((typeof(expr1) *)0 != (typeof(expr2) *)0)
#else /* HAVE_TYPEOF */
/* 
 * Without typeof, we can only test the sizes
 * to try and guess if the types match.
 */
#define check_types_match(expr1, expr2) \
        BUILD_ASSERT_OR_ZERO(sizeof(expr1) == sizeof(expr2))

#endif /* HAVE_TYPEOF */



/**
 * VARIABLE
 * ````````
 * Test that a macro argument is a variable.
 *
 * @v: Macro argument, maybe a variable, but maybe not! 
 *
 * NOTES
 * The trick is to declare an enumeration inside a new scope,
 * with the same name as the (potential) variable. Failure
 * will generate a compiler error.
 */
#ifndef VARIABLE
#define VARIABLE(v)     { enum v { }; }
#endif


/**
 * V_ASSIGN
 * ````````
 * Assign a value to a variable inside a macro.
 *
 * @variable: Macro argument, hopefully a variable.
 * @value   : Value to be assigned to the variable.
 */
#define V_ASSIGN(variable, value) \
        VARIABLE(variable); variable = value;


/** 
 * __same_type
 * ```````````
 * Test that two types/vars (ignoring qualifiers) are the same.
 *
 * @a: Variable with some type.
 * @b: Variable with some type.
 */
#ifndef __same_type
#define __same_type(a,b) __builtin_types_compatible_p(typeof(a), typeof(b))
#endif


/**
 * __must_be_array
 * ```````````````
 * Test that a pointer was actually declared as an array (contiguous).
 *
 * @arr: Pointer to a potential array.
 *
 * NOTES
 * &a[0] degrades to a pointer, which is a different type than 
 * the address of a declared array. Usually the compiler will 
 * print a warning and move on, but BUILD_BUG_ON_ZERO ensures 
 * the whole thing gets called off.
 */
#ifndef __ENABLE_BUILD_BUGS 
#define __must_be_array(arr) 0
#else
#define __must_be_array(a) BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#endif


/**
 * ARRAY_SIZE
 * ``````````
 * Determine the number of elements in a dynamic array (at runtime!).
 *
 * @arr: Pointer to an array.
 *
 * CAVEAT
 * The array must have been declared as a bona fide array, e.g.
 *
 *      my_array[13];
 *
 * An allocated region of memory, which is usually "the same thing"
 * as an array, is not valid here.
 *
 * NOTES 
 * This one is a pretty venerable trick; the version from the Linux 
 * kernel polices a common mistake by testing that the argument be 
 * a true array.
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))
#endif


#include <stddef.h>
/**
 * container_of 
 * ````````````
 * Get pointer to enclosing structure
 *
 * @member_ptr     : pointer to the structure member
 * @containing_type: the type this member is within
 * @member         : the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does pointer
 * subtraction to return the pointer to the enclosing type.
 *
 * EXAMPLE
 *
 * struct foo {
 *       int field_a;
 *       int field_b;
 *       ...
 * };
 *
 * struct info {
 *       int some_other_field;
 *       struct foo my_foo;
 * };
 *
 * static struct info *foo_to_info(struct foo *foo)
 * {
 *       return container_of(foo, struct info, my_foo);
 * }
 */
#define container_of(member_ptr, containing_type, member)                   \
        ((containing_type *)                                                \
        ((char *)(member_ptr)                                               \
        - container_off(containing_type, member))                           \
        + check_types_match(*(member_ptr), ((containing_type *)0)->member))


/**
 * container_off 
 * `````````````
 * Get offset to enclosing structure
 *
 * @containing_type: the type this member is within
 * @member         : the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does
 * typechecking and figures out the offset to the enclosing type.
 *
 * EXAMPLE
 *
 * struct foo {
 *       int fielda;
 *       int fieldb;
 *       ...
 * };
 *
 * struct info {
 *       int some_other_field;
 *       struct foo my_foo;
 * };
 *
 * static struct info *foo_to_info(struct foo *foo)
 * {
 *       size_t off = container_off(struct info, my_foo);
 *       return (void *)((char *)foo - off);
 * }
 */
#define container_off(containing_type, member) \
        offsetof(containing_type, member)


/**
 * container_of_var 
 * ````````````````
 * Get pointer to enclosing structure using a variable
 *
 * @member_ptr   : pointer to the structure member
 * @container_var: a pointer of same type as this member's container
 * @member       : the name of this member within the structure.
 *
 * Given a pointer to a member of a structure, this macro does pointer
 * subtraction to return the pointer to the enclosing type.
 *
 * EXAMPLE
 *
 * static struct info *foo_to_i(struct foo *foo)
 * {
 *      struct info *i = container_of_var(foo, i, my_foo);
 *      return i;
 * }
 */
#if HAVE_TYPEOF
#define container_of_var(member_ptr, container_var, member) \
        container_of(member_ptr, typeof(*container_var), member)
#else
#define container_of_var(member_ptr, container_var, member) \
        ((void *)((char *)(member_ptr) - \
        container_off_var(container_var, member)))
#endif

/**
 * container_off_var 
 * `````````````````
 * Get offset of a field in enclosing structure
 *
 * @container_var: a pointer to a container structure
 * @member       : the name of a member within the structure.
 *
 * Given (any) pointer to a structure and a its member name, this
 * macro does pointer subtraction to return offset of member in a
 * structure memory layout.
 */
#if HAVE_TYPEOF
#define container_off_var(var, member) \
        container_off(typeof(*var), member)
#else
#define container_off_var(var, member) \
        ((char *)&(var)->member - (char *)(var))
#endif


/******************************************************************************
 * OPTIMIZATION
 ******************************************************************************/

/**
 * likely/unlikely
 * ```````````````
 * Branch prediction macros.
 *
 * NOTES
 * gcc will emit instructions causing the branch prediction to 
 * favor the instruction on the "likely" side, re-arranging the 
 * jumps so that it gets tested first.
 * 
 * USAGE
 *      if (unlikely(c < 4)) {
 *              special code
 *      }
 *
 * CAVEAT
 * There has been some evidence that performance improvements here are
 * negligible in all but the most special cases.
 */
#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif



/******************************************************************************
 * MATHS
 ******************************************************************************/

/**
 * _abs_
 * `````
 * Absolute value for quantities <= 32 bits.
 *
 * @x    : unsigned/signed long, short, and char.
 * Return: signed long
 */
#define _abs_(x)                                        \
({				                        \
        long ret;					\
        if (sizeof(x) == sizeof(long)) {		\
                long __x = (x);				\
                ret = (__x < 0) ? -__x : __x;		\
        } else {					\
                int __x = (x);				\
                ret = (__x < 0) ? -__x : __x;		\
        }						\
        ret;						\
})


/**
 * _abs64_
 * ```````
 * Absolute value for quantities <= 64 bits.
 *
 * @x    : uint64_t, long long, etc.
 * Return: uint64_t
 */
#define _abs64_(x)                                      \
({				                        \
	uint64_t __x = (x);			        \
	(__x < 0) ? -__x : __x;		                \
})



/**
 * min
 * ```
 * Find the minimum value between two varaibles.
 * 
 * @x: Numeric variable.
 * @y: Numeric variable.
 * 
 * NOTE
 * Performes strict type-checking. See the "unnecessary" 
 * pointer comparison.
 */
#define min(x, y)                               \
({				                \
	__typeof__ (x) _min1 = (x);		\
	__typeof__ (y) _min2 = (y);		\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2;          \
})


/**
 * max
 * ```
 * Find the maximum value between two variables.
 *
 * @x: Numeric variable.
 * @y: Numeric variable.
 * 
 * NOTE
 * Performes strict type-checking. See the "unnecessary" 
 * pointer comparison.
 */
#define max(x, y)                               \
({				                \
	__typeof__ (x) _max1 = (x);		\
	__typeof__ (y) _max2 = (y);		\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2;          \
})



/**
 * min3
 * ````
 * Find the minimum value among three varaibles.
 * 
 * @x: Numeric variable.
 * @y: Numeric variable.
 * @z: Numeric variable.
 * 
 * NOTE
 * Performes strict type-checking. See the "unnecessary" 
 * pointer comparison.
 */
#define min3(x, y, z)                                     \
({			                                  \
	__typeof__ (x) _min1 = (x);			  \
	__typeof__ (y) _min2 = (y);			  \
	__typeof__ (z) _min3 = (z);			  \
	(void) (&_min1 == &_min2);		          \
	(void) (&_min1 == &_min3);		          \
	_min1 < _min2 ? (_min1 < _min3 ? _min1 : _min3) : \
		(_min2 < _min3 ? _min2 : _min3);          \
})



/**
 * max3
 * ````
 * Find the maximum value among three varaibles.
 * 
 * @x: Numeric variable.
 * @y: Numeric variable.
 * @z: Numeric variable.
 * 
 * NOTE
 * Performes strict type-checking. See the "unnecessary" 
 * pointer comparison.
 */
#define max3(x, y, z)                                     \
({			                                  \
	__typeof__ (x) _max1 = (x);                       \
	__typeof__ (y) _max2 = (y);                       \
	__typeof__ (z) _max3 = (z);                       \
	(void) (&_max1 == &_max2)                         \
	(void) (&_max1 == &_max3)                         \
	_max1 > _max2 ? (_max1 > _max3 ? _max1 : _max3)   \
                      : (_max2 > _max3 ? _max2 : _max3);  \
})



/**
 * min_not_zero 
 * ````````````
 * Find the non-zero minimum value between two variables. 
 *
 * @x: Numeric variable. 
 * @y: Numeric variable. 
 *
 * NOTES
 * Performes strict type-checking. See the "unnecessary" 
 * pointer comparison.
 */
#define min_not_zero(x, y)                              \
({			                                \
	__typeof__ (x) __x = (x);			\
	__typeof__ (y) __y = (y);			\
	__x == 0 ? __y                                  \
                 : ((__y == 0) ? __x : min(__x, __y));  \
})



/**
 * clamp 
 * `````
 * Clamp the value of a variable to a given range.
 *
 * @val: Numeric variable. 
 * @min: Minimum allowable value.
 * @max: Maximum allowable value.
 *
 * NOTES
 * Performes strict type-checking. See the "unnecessary" 
 * pointer comparison.
 */
#define clamp(val, min, max)                    \
({			                        \
	__typeof__ (val) __val = (val);		\
	__typeof__ (min) __min = (min);		\
	__typeof__ (max) __max = (max);		\
	(void) (&__val == &__min);		\
	(void) (&__val == &__max);		\
	__val = __val < __min ? __min: __val;	\
	__val > __max ? __max: __val;           \
})



/**
 * min_t
 * `````
 * Find the minimum value between two varaibles of given type.
 * 
 * @type: Type of the variables.
 * @x   : Numeric variable.
 * @y   : Numeric variable.
 *
 * NOTE
 * This macro performs no type-checking, and uses temporary 
 * variables of type 'type' to make all comparisons.
 */
#define min_t(type, x, y)                       \
({			                        \
	type __min1 = (x);                      \
	type __min2 = (y);                      \
	__min1 < __min2 ? __min1: __min2;       \
})


/**
 * max_t
 * `````
 * Find the maximum value between two varaibles of given type.
 * 
 * @type: Type of the variables.
 * @x   : Numeric variable.
 * @y   : Numeric variable.
 *
 * NOTE
 * This macro performs no type-checking, and uses temporary 
 * variables of type 'type' to make all comparisons.
 */
#define max_t(type, x, y)                       \
({			                        \
	type __max1 = (x);			\
	type __max2 = (y);			\
	__max1 > __max2 ? __max1: __max2;       \
})



/**
 * clamp_t 
 * ```````
 * Clamp the value of a variable to a given range.
 *
 * @type: Type of @val.
 * @val : Numeric variable. 
 * @min : Minimum allowable value.
 * @max : Maximum allowable value.
 *
 * NOTE
 * This macro performs no type-checking, and uses temporary 
 * variables of type 'type' to make all comparisons.
 */
#define clamp_t(type, val, min, max)            \
({		                                \
	type __val = (val);			\
	type __min = (min);			\
	type __max = (max);			\
	__val = __val < __min ? __min: __val;	\
	__val > __max ? __max: __val;           \
})



/**
 * clamp_val 
 * `````````
 * Clamp the value of a variable clamped to a given range of the same type.
 *
 * @val: Numeric variable 
 * @min: Minimum allowable value
 * @max: Maximum allowable value
 *
 * NOTE
 * This macro performs no type-checking, and uses temporary 
 * variables of the same type as @val to make all comparisons.
 * 
 * This is useful when @val is an unsigned type and @min/@max 
 * are literals that would otherwise be assigned a signed integer 
 * type.
 */
#define clamp_val(val, min, max)                \
({		                                \
	__typeof__ (val) __val = (val);		\
	__typeof__ (val) __min = (min);		\
	__typeof__ (val) __max = (max);		\
	__val = __val < __min ? __min: __val;	\
	__val > __max ? __max: __val;           \
})






/**
 * swap 
 * ````
 * Swap the value of two variables.
 *
 * @a: Some variable.
 * @b: Some variable. 
 *
 * NOTE
 * Uses a temporary variable with type matching that of @a.
 */
#define swap(a, b)                           \
	do {                                 \
                __typeof__ (a) __tmp = (a);  \
                (a) = (b);                   \
                (b) = __tmp;                 \
                                             \
        } while (0)


/**
 * CEIL
 * ````
 * Ceiling function.
 */
#define CEIL(a, b) (((a) / (b)) + ((a % b) > 0 ? 1 : 0))


#define BIT2(x)         (     (x)  | (     (x) >> 1))
#define BIT4(x)         ( BIT2(x)  | ( BIT2(x) >> 2))
#define BIT8(x)         ( BIT4(x)  | ( BIT4(x) >> 4))
#define BIT16(x)        ( BIT8(x)  | ( BIT8(x) >> 8))
#define BIT32(x)        (BIT16(x)  | (BIT16(x) >> 16))

#define NEXT_POWER_OF_2(x)    (BIT32(x-1) + 1)



/******************************************************************************
 * BOUNDS-CHECKING
 ******************************************************************************/

/*
 * Safe decrement and increment; the value of x is modified
 */
#define decrement(x, min) x = ((x) > (min)) ? ((x)-1) : (x)
#define increment(x, max) x = ((x) < (max)) ? ((x)+1) : (x)
#define dec(x, min) x = ((x) > (min)) ? ((x)-1) : (x)
#define inc(x, max) x = ((x) < (max)) ? ((x)+1) : (x)



/******************************************************************************
 * Ex libri Alan Holub 
 ******************************************************************************/

#define TOOHIGH(a, p)           ((p) - (a) > (ARRAY_SIZE(a) - 1))
#define TOOLOW(a, p)            ((p) - (a) < 0)
#define INBOUNDS(a, p)          (!(TOOHIGH(a,p) || TOOLOW(a,p)))

/* 
 * Largest int available 
 * on a machine 
 */ 
#define LARGEST_INT             (int)(((unsigned)(~0)) >> 1)




/******************************************************************************
 * MISC. HORRORS 
 ******************************************************************************/

/**
 * VA_NUM_ARGS
 * Counts the number of VA_ARGS by means of an intense and heathen magic,
 * the particulars of which are not to be uttered here, nor anywhere else.
 */ 
#define NUM_ARGS(...) \
        NUM_ARGS_IMPL(__VA_ARGS__, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, \
                                   53, 52, 51, 50, 49, 48, 47, 46, 45, 44, \
                                   43, 42, 41, 40, 39, 38, 37, 36, 35, 34, \
                                   33, 32, 31, 30, 29, 28, 27, 26, 25, 24, \
                                   23, 22, 21, 20, 19, 18, 17, 16, 15, 14, \
                                   13, 12, 11, 10,  9,  8,  7,  6,  5,  4, \
                                    3,  2,  1)

#define NUM_ARGS_IMPL( _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
                       _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                       _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                       _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
                       _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
                       _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
                       _61, _62, _63, N, ...) N



/*
 * Allows you to suppress the "variable declared but not used" warning
 * for variables that are used, but perhaps by a macro or perhaps in
 * another scope... or just to make the compiler hush.
 */
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif




/* 
 * VA_COMMA surrounds its arguments (__VA_ARGS__) with 8 additional 
 * arguments: one empty argument before (doesn't have to be empty --
 * it's thrown away) and 6 commas and an empty argument after.
 * These 8 or more arguments are passed to GET_8TH_ARG, which, as 
 * its name implies, expands to the eighth argument. All other 
 * arguments are discarded.
 */
#define GET_8TH_ARG(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7
#define COMMA ,

#define VA_COMMA(...) \
        GET_8TH_ARG(/*empty*/, ##__VA_ARGS__, COMMA, COMMA, COMMA, COMMA, COMMA, COMMA, /*empty*/)


#define VA_IF_NOT_EMPTY(emit, ...) \
        GET_8TH_ARG(/*empty*/, ##__VA_ARGS__, emit, emit, emit, emit, emit, emit, /*empty*/)



#define VA_EMIT(emit, ...) \
        GET_8TH_ARG(/*empty*/, ##__VA_ARGS__, "", "", "", "", "", "", emit)



#define PRIVATE static
#define PUBLIC  extern


#define CAT(a,b) a ## b
#define STR(a)   #a
#define XSTR(a)  STR(a)
#define EXP(a)   a


#define SUDO_MESSAGE "(Permission denied) This operation must be performed as root."


#endif

