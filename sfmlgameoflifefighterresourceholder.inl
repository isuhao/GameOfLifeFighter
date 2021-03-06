#include <sstream>
#include <stdexcept>

///Adapted from https://github.com/SFML/SFML-Game-Development-Book
template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& filename)
{
  // Create and load resource
  std::unique_ptr<Resource> resource(new Resource());
  if (!resource->loadFromFile(filename))
    throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);

  // If loading successful, insert resource to map
  InsertResource(id, std::move(resource));
}

template <typename Resource, typename Identifier>
template <typename Parameter>
void ResourceHolder<Resource, Identifier>::Load(Identifier id, const std::string& filename, const Parameter& secondParam)
{
  // Create and load resource
  std::unique_ptr<Resource> resource(new Resource());
  if (!resource->loadFromFile(filename, secondParam))
    throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);

  // If loading successful, insert resource to map
  insertResource(id, std::move(resource));
}

template <typename Resource, typename Identifier>
Resource& ResourceHolder<Resource, Identifier>::Get(Identifier id)
{
  auto found = mResourceMap.find(id);
  assert(found != mResourceMap.end());

  return *found->second;
}

template <typename Resource, typename Identifier>
const Resource& ResourceHolder<Resource, Identifier>::Get(Identifier id) const
{
  auto found = mResourceMap.find(id);
  assert(found != mResourceMap.end());

  return *found->second;
}

template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::InsertResource(
  Identifier id,
  std::unique_ptr<Resource> resource
)
{
  // Insert and check success
  auto inserted = mResourceMap.insert(std::make_pair(id, std::move(resource)));
  if (!inserted.second)
  {
    std::stringstream msg;
    msg << __func__ << ": could not insert resource";
    throw std::logic_error(msg.str());
  }
}
