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

void AravisCamera::printFeatureInfo(cameraFeature_id_t featureId, const FeatureInfo & info)
{
    if (bool available = false; !hasFeature(featureId, &available) || !available)
    {
        return;
    }

    std::cout << "- " << info.featureName << " (ID " << featureId << "):\n";

    if (info.autoName)
    {
        if (FeatureMode mode; getMode(featureId, &mode))
        {
            std::string modeStr;

            switch (mode)
            {
                case MODE_AUTO: modeStr = "Auto"; break;
                case MODE_MANUAL: modeStr = "Manual"; break;
                default: modeStr = "Unknown";
            }

            std::cout << "  Mode: " << modeStr << "\n";
        }
    }

    if (info.enabledName)
    {
        if (bool isActive; getActive(featureId, &isActive))
        {
            std::cout << "  Status: " << (isActive ? "Enabled" : "Disabled") << "\n";
        }
    }

    if (double value; getFeature(featureId, &value))
    {
        std::cout << "  Current value: " << value << "\n";
    }

    if (double min, max; getFeatureLimits(featureId, &min, &max))
    {
        std::cout << "  Range: " << min << " to " << max << "\n";
    }

    if (bool compatible; checkEnabled(featureId, &compatible))
    {
        std::cout << "  Compatible with current format: " << (compatible ? "yes" : "no") << "\n";
    }

    if (bool jauto; hasAuto(featureId, &jauto))
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
    cameraFeature_id_t id = id_find(featureName);

    if (id == YARP_FEATURE_INVALID)
    {
        std::cout << "Feature not found: " << featureName << "\n";
        return false;
    }

    return getFeature(id, &value);
}

cameraFeature_id_t AravisCamera::id_find(const std::string & feature_name)
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

    return YARP_FEATURE_INVALID;
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

            cameraFeature_id_t id = id_find(tokens[1]);

            if (id == YARP_FEATURE_INVALID)
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
