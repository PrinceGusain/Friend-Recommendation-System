#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <set>
#include <cmath>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

// Structure to store a friend's name and the interaction date
struct Connection
{
    string friendName;
    string date; // format: YYYY-MM-DD
};

// Graph to store friends and their interactions
unordered_map<string, vector<Connection>> graph;

// Function to convert date string to time_t
time_t parseDate(const string &dateStr)
{
    tm t = {};
    stringstream ss(dateStr);
    ss >> get_time(&t, "%Y-%m-%d");
    return mktime(&t);
}

// Load data from file into the graph
void loadGraph(const string &filename)
{
    ifstream file(filename);
    string u, v, date;

    while (file >> u >> v >> date)
    {
        graph[u].push_back({v, date});
        graph[v].push_back({u, date}); // undirected graph
    }

    file.close();
}

// Function to calculate time decay score
double timeDecayScore(const string &dateStr, double lambda = 0.05)
{
    time_t now = time(0); // current time
    time_t interactionTime = parseDate(dateStr);

    double daysPassed = difftime(now, interactionTime) / (60 * 60 * 24);
    return exp(-lambda * daysPassed); // exponential decay
}

// Function to recommend friends (friend-of-friend) with time-decay scores
vector<pair<string, double>> recommendFriends(const string &user)
{
    set<string> visited;
    queue<pair<string, int>> q;
    unordered_map<string, double> scoreMap;

    visited.insert(user);
    q.push({user, 0}); // start BFS from user

    while (!q.empty())
    {
        string current = q.front().first;
        int depth = q.front().second;
        q.pop();

        if (depth >= 2)
            continue; // only go up to friends-of-friends

        for (Connection conn : graph[current])
        {
            string neighbor = conn.friendName;

            if (neighbor == user)
                continue;

            if (visited.find(neighbor) == visited.end())
            {
                if (depth == 1)
                {
                    double score = timeDecayScore(conn.date);
                    scoreMap[neighbor] += score;
                }

                visited.insert(neighbor);
                q.push({neighbor, depth + 1});
            }
        }
    }

    vector<pair<string, double>> recommendations;
    for (auto &entry : scoreMap)
    {
        recommendations.push_back(entry);
    }

    return recommendations;
}

// Main program
int main()
{
    loadGraph("friends.txt");

    string user;
    cout << "Enter your name: ";
    cin >> user;

    // If user not found
    if (graph.find(user) == graph.end())
    {
        cout << "User not found in network.\n";
        return 0;
    }

    // Get recommendations
    vector<pair<string, double>> recs = recommendFriends(user);

    cout << "\nRecommended friends for " << user << ":\n";

    if (recs.empty())
    {
        cout << "No recommendations found.\n";
    }
    else
    {
        ofstream out("output.txt");
        out << "Recommended friends for " << user << ":\n";

        for (pair<string, double> rec : recs)
        {
            out << rec.first << " (Score: " << fixed << setprecision(2) << rec.second << ")\n";
        }

        out.close();
        cout << "Recommendations written to output.txt\n";
    }

    return 0;
}
