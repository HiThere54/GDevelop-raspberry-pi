#ifndef GDEXPRESSION_H
#define GDEXPRESSION_H

#include <string>
#include <vector>
#include <iostream>
#include "GDL/GDMathParser.h"
#include <boost/shared_ptr.hpp>
#include "GDL/ExpressionInstruction.h"
#include "GDL/ObjectIdentifiersManager.h"
class Object;
class RuntimeScene;
class ObjectsConcerned;

typedef boost::shared_ptr<Object> ObjSPtr;


/**
 * Class representing an expression.
 * Hold the plain expression string, the functions to call
 * beforing evaluating the expression with a parser, .
 */
class GD_API GDExpression
{
    public:
        GDExpression(std::string plainString_);
        virtual ~GDExpression();

        /**
         * Get the plain string representing the expression
         */
        inline std::string GetPlainString() const { return plainString; };

        /**
         * Get a number representing a comparision operator.
         */
        inline short int GetAsCompOperator() const { return compOperator; };

        /**
         * Get a number representing a modification operator.
         */
        inline short int GetAsModOperator() const { return modOperator; };

        /**
         * Get the object identifier representing the object
         */
        inline unsigned int GetAsObjectIdentifier() const
        {
            if ( !oIDcomputed )
            {
                ObjectIdentifiersManager * objectIdentifiersManager = ObjectIdentifiersManager::getInstance();
                oID = objectIdentifiersManager->GetOIDfromName(plainString);
                oIDcomputed = true;
            }

            return oID;
        }

        /**
         * Evaluate as a math expression and return the result
         */
        double GetAsMathExpressionResult(RuntimeScene & scene,
                                         ObjectsConcerned & objectsConcerned,
                                         ObjSPtr obj1 = boost::shared_ptr<Object>( ),
                                         ObjSPtr obj2 = boost::shared_ptr<Object>( )) const
         {
            vector < double > parametersValues;

            for (unsigned int i = 0;i<mathExpressionFunctions.size();++i)
                parametersValues.push_back((mathExpressionFunctions[i].function)(scene, objectsConcerned, obj1, obj2, mathExpressionFunctions[i]));

            if ( parametersValues.empty() ) parametersValues.push_back(0);

            return mathExpression.Eval(&parametersValues[0]);
         }

        /**
         * Evaluate as a text expression and return the result
         */
        std::string GetAsTextExpressionResult(RuntimeScene & scene,
                                              ObjectsConcerned & objectsConcerned,
                                              ObjSPtr obj1 = boost::shared_ptr<Object>( ),
                                              ObjSPtr obj2 = boost::shared_ptr<Object>( )) const;

        /**
         * Preprocess expressions in order to allow evaluation as text or math expression.
         */
        bool PreprocessExpressions(RuntimeScene & scene);

        /**
         * Add a function to call to evaluate parameters of mathExpression.
         */
        inline void AddMathExprFunction(const ExpressionInstruction & exprInstruction) { mathExpressionFunctions.push_back(exprInstruction); };

        /**
         * Get the functions to call to evaluate parameters of mathExpression.
         */
        inline const std::vector < ExpressionInstruction > & GetMathExprFunctions() const { return mathExpressionFunctions; }

        enum compOperator
        {
            Equal, Inferior, Superior, InferiorOrEqual, SuperiorOrEqual, Different, Undefined
        };

        enum modOperator
        {
            Set, Add, Substract, Multiply, Divide, UndefinedModification
        };


    protected:
    private:

        /**
         * Preprocess the math expression, in order to allow its evaluation.
         */
        bool PreprocessMathExpression(RuntimeScene & scene);

        /**
         * Preprocess the text expression, in order to allow its evaluation.
         */
        bool PreprocessTextExpression(RuntimeScene & scene);

        std::string     plainString; ///<The plain expression
        char            compOperator; ///<Char representing a comparison operator. Computed at creation.
        char            modOperator; ///<Char representing a modification operator. Computed at creation.
        mutable unsigned int    oID; ///< Object identifier, if expression contains an object name. Automatically computed when needed.
        mutable bool    oIDcomputed;

        GDMathParser  mathExpression; ///<Object representing the mathemathic expression to parse and evaluate.
        std::vector < ExpressionInstruction > mathExpressionFunctions; ///< The functions to call to generate the values of the parameters to pass to the mathematic expression when evaluating.
        bool            isMathExpressionPreprocessed; ///<Indicate if the functions to call and the mathematic expression have been preprocessed.

};

#endif // GDEXPRESSION_H
