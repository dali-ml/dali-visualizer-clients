#ifndef DALI_VISUALIZER_VISUALIZER_H
#define DALI_VISUALIZER_VISUALIZER_H

#include "dali/config.h"

#include <dali/tensor/Mat.h>
#include <dali/utils/core_utils.h>
#include <dali/utils/Reporting.h>
#include <json11.hpp>
#include <redox.hpp>
#include <memory>
#include <chrono>
#include <functional>
#include <string>

#include "dali_visualizer/EventQueue.h"

// to import Throttled

// TODO: Szymon explain how this works
namespace dali {
    namespace visualizer {

        struct Visualizable {
            virtual json11::Json to_json() = 0;
        };

        template<typename R>
        struct Sentence : public Visualizable {
            std::vector<std::string> tokens;
            std::vector<R> weights;
            bool spaces = true;

            Sentence(std::vector<std::string> tokens);

            void set_weights(const std::vector<R>& _weights);
            void set_weights(const Mat<R>& _weights);

            virtual json11::Json to_json() override;
        };

        template<typename R>
        std::vector<std::shared_ptr<Sentence<R>>> sentence_vector(const std::vector<std::vector<std::string>>& vec);

        template<typename R>
        struct Sentences : public Visualizable {
            typedef std::shared_ptr<Sentence<R>> sentence_ptr;
            std::vector<sentence_ptr> sentences;
            std::vector<R> weights;

            Sentences(std::vector<sentence_ptr> sentences);
            Sentences(std::vector<std::vector<std::string>> vec);

            void set_weights(const std::vector<R>& _weights);
            void set_weights(const Mat<R>& _weights);

            virtual json11::Json to_json() override;
        };

        template<typename R>
        struct ParallelSentence : public Visualizable {
            typedef std::shared_ptr<Sentence<R>> sentence_ptr;
            sentence_ptr sentence1;
            sentence_ptr sentence2;
            ParallelSentence(sentence_ptr sentence1, sentence_ptr sentence2);
            virtual json11::Json to_json() override;
        };

        template<typename R>
        struct QA : public Visualizable {
            typedef std::shared_ptr<Sentence<R>> sentence_ptr;
            typedef std::shared_ptr<Visualizable> visualizable_ptr;
            visualizable_ptr context;
            sentence_ptr question;
            sentence_ptr answer;

            QA(visualizable_ptr context, sentence_ptr question, sentence_ptr answer);

            virtual json11::Json to_json() override;
        };

        struct GridLayout : public Visualizable {
            typedef std::shared_ptr<Visualizable> visualizable_ptr;

            std::vector<std::vector<visualizable_ptr>> grid;
            visualizable_ptr output;

            // adds a card in <column>-th column
            void add_in_column(int column, visualizable_ptr);

            virtual json11::Json to_json() override;
        };

        template<typename R>
        struct FiniteDistribution : public Visualizable {
            static const std::vector<R> empty_vec;
            std::vector<R> distribution;
            std::vector<R> scores;
            std::vector<std::string> labels;
            int top_picks;

            FiniteDistribution(const std::vector<R>& distribution,
                               const std::vector<R>& scores,
                               const std::vector<std::string>& labels,
                               int max_top_picks = -1);

            FiniteDistribution(const std::vector<R>& distribution,
                   const std::vector<std::string>& labels,
                   int max_top_picks = -1);

            virtual json11::Json to_json() override;
        };

        template<typename T>
        struct Probability: public Visualizable {
            T probability;

            Probability(T probability);

            virtual json11::Json to_json() override;
        };

        struct Message: public Visualizable {
            std::string content;

            Message(std::string content);

            virtual json11::Json to_json() override;
        };

        struct Tree: public Visualizable {
            std::string label;
            std::vector<std::shared_ptr<Tree>> children;

            Tree(std::string label);

            Tree(std::initializer_list<std::shared_ptr<Tree>> children);

            Tree(std::vector<std::shared_ptr<Tree>> children);

            Tree(std::string label, std::initializer_list<std::shared_ptr<Tree>> children);

            Tree(std::string label, std::vector<std::shared_ptr<Tree>> children);

            virtual json11::Json to_json() override;
        };

        template<typename R>
        json11::Json json_finite_distribution(const Mat<R>&, const std::vector<std::string>& labels);

        template<typename R>
        json11::Json json_classification(const std::vector<std::string>& sentence, const Mat<R>& probs, const std::vector<std::string>& label_names);

        template<typename R>
        json11::Json json_classification(const std::vector<std::string>& sentence, const Mat<R>& probs, const Mat<R>& word_weights, const std::vector<std::string>& label_names);

        // TODO: explain what this does
        class Visualizer {
            public:
                typedef std::function<void(std::string,json11::Json)> function_t;
                const std::string hostname;
                const int port;
            private:
                bool subscription_active = false;
                bool running;

                std::string my_uuid;
                std::string my_name;

                std::shared_ptr<redox::Redox> rdx;
                std::shared_ptr<redox::Subscriber> callcenter_main_phoneline;

                Throttled throttle;

                std::mutex connection_mutex;
                std::shared_ptr<std::thread> ping_thread;

                std::mutex callcenter_mutex;
                std::unordered_map<std::string, function_t> callcenter_name_to_lambda;

                std::atomic<int> rdx_state;
                std::atomic<int> callcenter_state;

                void update_subscriber();

                void rdx_connected_callback(int status);
                void callcenter_connected_callback(int status);
                bool ensure_connection();
                void ping();
                bool verify_subscription_active();
            public:
                void whoami(std::string, json11::Json);

                void register_function(std::string name,  function_t lambda);

                Visualizer(std::string name, std::string hostname, int port);
                ~Visualizer();

                void feed(const json11::Json& obj);
                void feed(const std::string& str);
                void throttled_feed(Throttled::Clock::duration time_between_feeds, std::function<json11::Json()> f);
        };
    }
}
#endif
