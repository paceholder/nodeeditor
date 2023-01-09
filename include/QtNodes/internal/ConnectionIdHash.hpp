#pragma once

#include <functional>

#include "Definitions.hpp"

inline void hash_combine(std::size_t &seed)
{
    Q_UNUSED(seed);
}

template<typename T, typename... Rest>
inline void hash_combine(std::size_t &seed, const T &v, Rest... rest)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}

namespace std {
template<>
struct hash<QtNodes::ConnectionId>
{
    inline std::size_t operator()(QtNodes::ConnectionId const &id) const
    {
        std::size_t h = 0;
        hash_combine(h, id.outNodeId, id.outPortIndex, id.inNodeId, id.inPortIndex);
        return h;
    }
};

template<>
struct hash<std::pair<QtNodes::NodeId, QtNodes::PortIndex>>
{
    inline std::size_t operator()(std::pair<QtNodes::NodeId, QtNodes::PortIndex> const &nodePort) const
    {
        std::size_t h = 0;
        hash_combine(h, nodePort.first, nodePort.second);
        return h;
    }
};

template<>
struct hash<std::tuple<QtNodes::NodeId, QtNodes::PortType, QtNodes::PortIndex>>
{
    using Key = std::tuple<QtNodes::NodeId, QtNodes::PortType, QtNodes::PortIndex>;

    inline std::size_t operator()(Key const &key) const
    {
        std::size_t h = 0;
        hash_combine(h, std::get<0>(key), std::get<1>(key), std::get<2>(key));
        return h;
    }
};
} // namespace std
