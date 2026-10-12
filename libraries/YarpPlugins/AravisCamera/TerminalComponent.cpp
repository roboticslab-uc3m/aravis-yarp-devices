#include "AravisCamera.hpp"

#include <cstdlib> // std::exit

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string> // std::stod
#include <thread>
#include <vector>

namespace
{
    void get_command(const std::string & command, std::vector<std::string> & tokens)
    {
        std::stringstream ss(command);
        std::string token;

        while (ss >> token)
        {
            tokens.push_back(token);
        }
    }
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
void AravisCamera::printFeatureInfo(yarp::dev::cameraFeature_id_t featureId, const FeatureInfo & info)
#else
void AravisCamera::printFeatureInfo(cameraFeature_id_t featureId, const FeatureInfo & info)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (bool available = false; !hasFeature(featureId, available) || !available)
#else
    if (bool available = false; !hasFeature(featureId, &available) || !available)
#endif
    {
        return;
    }

    std::cout << "- " << info.featureName << " (ID " << static_cast<int>(featureId) << "):\n";

    if (info.autoName)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        if (yarp::dev::FeatureMode mode; getMode(featureId, mode))
#else
        if (FeatureMode mode; getMode(featureId, &mode))
#endif
        {
            std::string modeStr;

            switch (mode)
            {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
                case yarp::dev::FeatureMode::MODE_AUTO: modeStr = "Auto"; break;
                case yarp::dev::FeatureMode::MODE_MANUAL: modeStr = "Manual"; break;
#else
                case MODE_AUTO: modeStr = "Auto"; break;
                case MODE_MANUAL: modeStr = "Manual"; break;
#endif
                default: modeStr = "Unknown";
            }

            std::cout << "  Mode: " << modeStr << "\n";
        }
    }

    if (info.enabledName)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        if (bool isActive; getActive(featureId, isActive))
#else
        if (bool isActive; getActive(featureId, &isActive))
#endif
        {
            std::cout << "  Status: " << (isActive ? "Enabled" : "Disabled") << "\n";
        }
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (double value; getFeature(featureId, value))
#else
    if (double value; getFeature(featureId, &value))
#endif
    {
        std::cout << "  Current value: " << value << "\n";
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (double min, max; getFeatureLimits(featureId, min, max))
#else
    if (double min, max; getFeatureLimits(featureId, &min, &max))
#endif
    {
        std::cout << "  Range: " << min << " to " << max << "\n";
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (bool compatible; checkEnabled(featureId, compatible))
#else
    if (bool compatible; checkEnabled(featureId, &compatible))
#endif
    {
        std::cout << "  Compatible with current format: " << (compatible ? "yes" : "no") << "\n";
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (bool jauto; hasAuto(featureId, jauto))
#else
    if (bool jauto; hasAuto(featureId, &jauto))
#endif
    {
        std::cout << "  Has auto: " << (jauto ? "yes" : "no") << "\n";
    }
}

void AravisCamera::listAvailableFeatures()
{
    std::cout << "Listing available features:\n";

    for (const auto & [id, info] : yarp_arv_int_feature_map)
    {
        printFeatureInfo(id, info);
    }

    for (const auto & [id, info] : yarp_arv_float_feature_map)
    {
        printFeatureInfo(id, info);
    }
}

bool AravisCamera::checkFeatureExistenceAndGetValue(const std::string & featureName, double & value)
{
    auto id = id_find(featureName);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (id == yarp::dev::cameraFeature_id_t::YARP_FEATURE_INVALID)
#else
    if (id == YARP_FEATURE_INVALID)
#endif
    {
        std::cout << "Feature not found: " << featureName << "\n";
        return false;
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return getFeature(id, value);
#else
    return getFeature(id, &value);
#endif
}

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::cameraFeature_id_t AravisCamera::id_find(const std::string & feature_name)
#else
cameraFeature_id_t AravisCamera::id_find(const std::string & feature_name)
#endif
{
    for (const auto [id, info] : yarp_arv_int_feature_map)
    {
        if (std::string(info.featureName) == feature_name)
        {
            return id;
        }
    }

    for (const auto [id, info] : yarp_arv_float_feature_map)
    {
        if (std::string(info.featureName) == feature_name)
        {
            return id;
        }
    }


#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::cameraFeature_id_t::YARP_FEATURE_INVALID;
#else
    return YARP_FEATURE_INVALID;
#endif
}

void AravisCamera::runInteractiveTerminal()
{
    std::string command;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    while (true)
    {
        std::cout << "\n> ";
        std::getline(std::cin, command);

        std::vector<std::string> tokens;
        get_command(command, tokens);

        if (tokens.empty())
        {
            continue;
        }

        std::string cmd = tokens[0];

        if (cmd == "exit")
        {
            std::cout << "Exiting interactive mode...\n";
            close();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::exit(0);
        }
        else if (cmd == "list_features")
        {
            listAvailableFeatures();
        }
        else if (cmd == "get_feature")
        {
            if (tokens.size() != 2)
            {
                std::cout << "Usage: get_feature <FeatureName>\n";
                continue;
            }

            if (double value; checkFeatureExistenceAndGetValue(tokens[1], value))
            {
                std::cout << "Feature " << tokens[1] << " value: " << value << "\n";
            }
            else
            {
                std::cout << "Failed to get feature " << tokens[1] << "\n";
            }
        }
        else if (cmd == "set_feature")
        {
            if (tokens.size() != 3)
            {
                std::cout << "Usage: set_feature <FeatureName> <value>\n";
                continue;
            }

            double value;

            try
            {
                value = std::stod(tokens[2]);
            }
            catch (const std::exception &e)
            {
                std::cout << "Invalid value format.\n";
                continue;
            }

            auto id = id_find(tokens[1]);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
            if (id == yarp::dev::cameraFeature_id_t::YARP_FEATURE_INVALID)
#else
            if (id == YARP_FEATURE_INVALID)
#endif
            {
                std::cout << "Feature not found: " << tokens[1] << "\n";
                continue;
            }

            if (setFeature(id, value))
            {
                std::cout << "Feature " << tokens[1] << " set to: " << value << "\n";
            }
            else
            {
                std::cout << "Failed to set feature " << tokens[1] << "\n";
            }
        }
        else if (cmd == "help")
        {
            std::cout << "Available commands:\n"
                      << "  list_features\n"
                      << "  get_feature <FeatureName>\n"
                      << "  set_feature <FeatureName> <value>\n"
                      << "  exit\n";
        }
        else
        {
            std::cout << "Unknown command. Type 'help' for available commands.\n";
        }
    }
}
