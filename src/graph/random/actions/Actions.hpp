#ifndef GRPH_ACTIONS_HPP
#define GRPH_ACTIONS_HPP

#include "../IRandomGraph.hpp"

namespace grph::graph::random {

    class RollbackAction {
    public:
        explicit RollbackAction(
                const std::shared_ptr<DegreesVector> &degreesVectorBeforeAction
        ) : _degreesVectorBeforeAction(degreesVectorBeforeAction) {};

        virtual void rollback() = 0;

        virtual ~RollbackAction() = default;

    protected:
        std::shared_ptr<DegreesVector> _degreesVectorBeforeAction;
    };

    class RemoveRandomEdgeAction : public RollbackAction {
    public:
        explicit RemoveRandomEdgeAction(
                const std::shared_ptr<DegreesVector> &degreesVectorBeforeAction
        ) : RollbackAction(degreesVectorBeforeAction) {};

        void rollback() override = 0;

        virtual Edge getRemovedEdge() const = 0;

        virtual double getEdgeProbability() const = 0;

        ~RemoveRandomEdgeAction() override = default;
    };

    class PullEdgeAction : public RollbackAction {
    public:
        explicit PullEdgeAction(
                const std::shared_ptr<DegreesVector> &degreesVectorBeforeAction
        ) : RollbackAction(degreesVectorBeforeAction) {};

        void rollback() override = 0;

        ~PullEdgeAction() override = default;
    };

    class RemoveTrivialChainAction : public RollbackAction {
    public:
        explicit RemoveTrivialChainAction(
                const std::shared_ptr<DegreesVector> &degreesVectorBeforeAction
        ) : RollbackAction(degreesVectorBeforeAction) {};

        void rollback() override = 0;

        virtual double getChainFactor() const = 0;

        ~RemoveTrivialChainAction() override = default;
    };

}

#endif //GRPH_ACTIONS_HPP
