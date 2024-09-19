//
// Created by 28192 on 2024/9/14.
//

#ifndef PINECONE_COPYABLE_H
#define PINECONE_COPYABLE_H

namespace pinecone {
/// A tag class emphasises the objects are Copyable.
/// The empty base class optimization applies.
/// Any derived class of Copyable should be a value type.
class Copyable {
 protected:
  Copyable() = default;

  ~Copyable() = default;
};
}  // namespace pinecone

#endif  // PINECONE_COPYABLE_H
