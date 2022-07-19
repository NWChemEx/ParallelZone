#include <parallelzone/runtime/resource_set.hpp>
#include <vector>

namespace parallelzone::runtime {
namespace detail_ {

struct ResourceSetPIMPL {
    using parent_type = ResourceSet;

    using worker_type = parent_type::worker_type;

    using worker_container = std::vector<worker_type>;

    using pimpl_pointer = parent_type::pimpl_pointer;

    worker_container m_workers;

    pimpl_pointer clone() const {
        return std::make_unique<ResourceSetPIMPL>(*this);
    }
};

} // namespace detail_

// -----------------------------------------------------------------------------
// -- Ctors, assignment, and dtor
// -----------------------------------------------------------------------------

ResourceSet::ResourceSet() noexcept = default;

ResourceSet::ResourceSet(pimpl_pointer pimpl) noexcept :
  m_pimpl_(std::move(pimpl)) {}

ResourceSet::ResourceSet(const ResourceSet& other) :
  m_pimpl_(other.m_pimpl_ ? other.m_pimpl_->clone() : nullptr) {}

ResourceSet::ResourceSet(ResourceSet&& other) noexcept = default;

ResourceSet& ResourceSet::operator=(const ResourceSet& rhs) {
    if(this != &rhs) { ResourceSet(rhs).swap(*this); }
    return *this;
}

ResourceSet& ResourceSet::operator=(ResourceSet&& rhs) noexcept = default;

ResourceSet::~ResourceSet() noexcept = default;

// -----------------------------------------------------------------------------
// -- Getters
// -----------------------------------------------------------------------------

ResourceSet::size_type ResourceSet::n_workers() const noexcept {
    return !empty() ? m_pimpl_->m_workers.size() : 0;
}

bool ResourceSet::has_me() const { throw std::runtime_error("NYI"); }

ResourceSet::worker_type ResourceSet::me() { throw std::runtime_error("NYI"); }

// -----------------------------------------------------------------------------
// -- Utility methods
// -----------------------------------------------------------------------------

bool ResourceSet::empty() const noexcept {
    return !static_cast<bool>(m_pimpl_);
}

void ResourceSet::swap(ResourceSet& other) noexcept {
    m_pimpl_.swap(other.m_pimpl_);
}

bool ResourceSet::operator==(const ResourceSet& rhs) const noexcept {
    // Check if one is empty and other isn't
    if(empty() != rhs.empty()) return false;

    // Both are empty, or both are non-empty, if empty return
    if(empty()) return true;

    return m_pimpl_->m_workers == rhs.m_pimpl_->m_workers;
}

bool ResourceSet::operator!=(const ResourceSet& rhs) const noexcept {
    return !(*this == rhs);
}

} // namespace parallelzone::runtime
